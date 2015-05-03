# $Id$
# $(c)$

class TireSamplesController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

  before_action :authenticate_user!, except: [:create]

  def index
    smart_listing_create partial: 'tire_samples/list'
  end

  def new
    @tire_sample = TireSample.new
    @tire_sample.sample_time = DateTime.now
  end

  def create
    fail NewcoError::VerificationError.new(request) unless
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
    @tire_sample ||= params[:id] ? TireSample.find(params[:id]) : TireSample.new(params[:tire])
  end
  helper_method :smart_listing_resource

  def smart_listing_collection
    scoped_tire_samples = \
    if current_user.global_admin?
      TireSample.all_samples
    else
      TireSample.all_samples_for_company(current_user.company_id)
    end

    scoped_tire_samples = scoped_tire_samples.contains(params[:filter]) if params[:filter]

    scoped_tire_samples = scoped_tire_samples.where("value <= ?", params[:max_val]) if \
      params[:max_val] && !params[:max_val].blank?
    scoped_tire_samples = scoped_tire_samples.where("value >= ?", params[:min_val]) if \
      params[:min_val] && !params[:min_val].blank?

    scoped_tire_samples = scoped_tire_samples.where("sample_time <= ?", params[:max_dt]) if \
      params[:max_dt] && !params[:max_dt].blank?
    scoped_tire_samples = scoped_tire_samples.where("sample_time >= ?", params[:min_dt]) if \
      params[:min_dt] && !params[:min_dt].blank?

    @tire_samples ||= scoped_tire_samples
  end
  helper_method :smart_listing_collection

  def self.generate_verify_value(tmp_params)
    tmp_tire_sample_params = tmp_params[:tire_sample].slice(:sensor_id, :receiver_id, :value, :sample_time)
    tire_param_str = tmp_tire_sample_params.values.map { |val| val.to_s }.sort.join('_')
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
