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
    return unless current_user.global_admin? || confirm_verify_value
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
    scoped_tire_samples = TireSample.all_samples

    scoped_tire_samples = scoped_tire_samples.contains(params[:filter]) if params[:filter]

    @tire_samples ||= scoped_tire_samples
  end
  helper_method :smart_listing_collection

  def self.generate_verify_value(tmp_params)
    tmp_tire_sample_params = tmp_params[:tire_sample].slice(:sensor_id, :receiver_id, :value, :sample_time)
    tire_param_str = tmp_tire_sample_params.values.map { |val| val.to_s }.sort.join('_')
    verify_val = BCrypt::Password.create(tire_param_str, :cost => 10)
  end

  delegate :generate_verify_value, to: :class
  helper_method :generate_verify_value

  private

  def find_tire_sample
    @tire_sample = TireSample.find(params[:id])
  end

  def tire_sample_params
    params.require(:tire_sample).permit(:sensor_id, :receiver_id, :value, :sample_time, :verify, :current_user)
  end

  def confirm_verify_value
    generate_verify_value(tire_sample_params) == tire_sample_params[:verify]
  end
end
