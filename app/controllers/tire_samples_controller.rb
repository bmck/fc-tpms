# $Id$
# $(c)$

class TireSamplesController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

  def index
    @tire_samples = smart_listing_create partial: 'tire_sample/list'
  end

  def new
    @tire_sample = TireSample.new
  end

  def create
    @tire_sample = TireSample.create(tire_sample_params)
  end

  def edit
  end

  def update
    smart_listing_resource.update_attributes(tire_sample_params)
  end

  def destroy
    smart_listing_resource.destroy
  end

  def smart_listing_resource
    @tire_sample ||= params[:id] ? TireSample.find(params[:id]) : TireSample.new(params[:tire])
  end
  helper_method :smart_listing_resource

  def smart_listing_collection
    scoped_tire_samples = TireSample.all_tire_samples

    scoped_tire_samples = scoped_tire_samples.contains(params[:filter]) if params[:filter]

    @tire_samples = scoped_tire_samples
  end
  helper_method :smart_listing_collection

  private

  def find_tire_sample
    @tire_sample = TireSample.find(params[:id])
  end

  def tire_sample_params
    params.require(:tire_sample).permit(:sensor_id, :receiver_id, :value, :sample_time)
  end
end
