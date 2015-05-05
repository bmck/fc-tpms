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
                        [:val, :value], [:samptime, :sample_time]]
      end

  def new
    @tire_sample = TireSample.new
    @tire_sample.sample_time = DateTime.now
  end

  def create
    fail NewcoError::VerificationError.new(request) unless \
      current_user.global_admin? || confirm_verify_value

    @tire_sample = TireSample.create(tire_sample_params)
  end

  def edit
  end

  def update
    @tire_sample.update_attributes(tire_sample_params)
  end

  def destroy
    @tire_sample.destroy
  end

  def smart_listing_resource
    @tire_sample ||= (params[:id] ? TireSample.find(params[:id]) : TireSample.new(params[:tire_sample]))
  end
  helper_method :smart_listing_resource

  def smart_listing_collection
    scoped_tire_samples = TireSample.all_samples #.ordered_samples

    scoped_tire_samples = scoped_tire_samples.company(current_user.company_id) unless \
      current_user.global_admin?

    scoped_tire_samples = scoped_tire_samples.contains(params[:filter]) unless \
      params[:filter].blank?

    if !params[:min_val].blank? && !params[:max_val].blank? && \
        params[:min_val].to_f > params[:max_val].to_f
      mn, mx = params[:max_val].to_f, params[:min_val].to_f
      params[:min_val], params[:max_val] = mn, mx
    end

    scoped_tire_samples = scoped_tire_samples.max_val(params[:max_val]) unless \
      params[:max_val].blank?
    scoped_tire_samples = scoped_tire_samples.min_val(params[:min_val]) unless \
      params[:min_val].blank?

    @tire_samples ||= scoped_tire_samples
  end
  helper_method :smart_listing_collection

  def self.generate_verify_value(tmp_params)
    tmp_tire_sample_params = tmp_params[:tire_sample].slice(
      :sensor_id, :receiver_id, :value, :sample_time
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
    params.require(:tire_sample).permit(:sensor_id, :receiver_id, :value, :sample_time, :verify)
  end

  def confirm_verify_value
    generate_verify_value(tire_sample_params) == tire_sample_params[:verify]
  end
end
