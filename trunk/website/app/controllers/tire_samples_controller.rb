# $Id$
# $(c)$

class TireSamplesController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

  before_action :find_tire_sample, except: [:index, :new, :create]
  before_action :authenticate_user!, except: [:create]

  def index
    smart_listing_create partial: 'tire_samples/list',
      sort_attributes: [[:tire_name, "tires.serial"], [:receiver_name, "receivers.serial"],
                        [:psi, :psi], [:samptime, :sample_time]],
      default_sort: { samptime: 'desc'}
  end

  def new
    @tire_sample = TireSample.new
    @tire_sample.sample_time = DateTime.now
  end

  def create
    # fail NewcoError::VerificationError.new(request) unless \
    #   current_user.global_admin? || confirm_verify_value

    @tire_sample = TireSample.create(tire_sample_params)
    if @tire_sample.errors.any?
      render \
        text: (['Sample creation failed.'] +
               Array(@tire_sample.errors.full_messages)).join('<br/>'+"\n"),
        status: 403 and return
    end

    respond_to do |format|
      format.html { render text: 'ok', status: 200 and return }
      format.js {  }
    end
  rescue ActiveRecord::RecordInvalid
    render text: 'fail', status: 403 and return
  end

  def edit
  end

  def update
    @tire_sample.update_attributes(tire_sample_params)
  end

  def destroy
    @tire_sample.destroy
  end

  def view_audit
    @tire_sample.touch_with_version if @tire_sample.versions.empty?
  end

  def smart_listing_resource
    @tire_sample ||= (params[:id] ? TireSample.find(params[:id]) :
                      TireSample.new(params[:tire_sample]))
  end
  helper_method :smart_listing_resource

  def smart_listing_collection
    scoped_tire_samples = TireSample.all_samples #.ordered_samples

    scoped_tire_samples = scoped_tire_samples.company(current_user.company_id) unless \
      current_user.global_admin?

    scoped_tire_samples = scoped_tire_samples.contains(params[:filter]) unless \
      params[:filter].blank?

    if !params[:min_psi].blank? && !params[:max_psi].blank? && \
        params[:min_psi].to_f > params[:max_psi].to_f
      mn, mx = params[:max_psi].to_f, params[:min_psi].to_f
      params[:min_psi], params[:max_psi] = mn, mx
    end

    scoped_tire_samples = scoped_tire_samples.max_val(params[:max_psi]) unless \
      params[:max_psi].blank?
    scoped_tire_samples = scoped_tire_samples.min_val(params[:min_psi]) unless \
      params[:min_psi].blank?

    @tire_samples ||= scoped_tire_samples
  end
  helper_method :smart_listing_collection

  def self.generate_verify_value(tmp_params)
    tmp_tire_sample_params = tmp_params[:tire_sample].slice(
      :sensor_id, :receiver_id, :psi, :sample_time
    )
    tire_param_str = tmp_tire_sample_params.values.map(&:to_s).sort.join('_')
    verify_val = BCrypt::Password.create(tire_param_str, cost: 10)
  end

  delegate :generate_verify_value, to: :class
  helper_method :generate_verify_value

  private

  def find_tire_sample
    @tire_sample = TireSample.find(params[:id])
  end

  def tire_sample_params
    begin
      params[:tire_sample][:psi] ||= params[:tire_sample][:value] if \
        params[:tire_sample][:value]
    rescue
    end

    params.require(:tire_sample).permit(:sensor_id, :receiver_id,
                                        :sample_time, :verify, :tempc, :psi)
  end

  def confirm_verify_value
    generate_verify_value(tire_sample_params) == tire_sample_params[:verify]
  end
end
