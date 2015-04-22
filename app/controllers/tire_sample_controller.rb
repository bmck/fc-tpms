# $Id$
# $(c)$

class TireSampleController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper
  helper_method :smart_listing_resource, :smart_listing_collection

  before_filter :smart_listing_resource, only: [:update, :destroy]

  def index
    @tire_samples = smart_listing_create partial: "tire_sample/list"
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
    @tire_sample.update_attributes(tire_sample_params)
  end

  def destroy
    @tire_sample.destroy
  end

  def smart_listing_resource
    @tire_sample ||= params[:id] ? TireSample.find(params[:id]) : TireSample.new(params[:tire])
  end

  def smart_listing_collection
    scoped_tire_samples = TireSample.all_tire_samples

    scoped_tire_samples = scoped_tire_samples.contains(params[:filter]) if params[:filter]

    @tire_samples ||= scoped_tire_samples
  end


  private

  def find_tire_sample
    @tire_sample = TireSample.find(params[:id])
  end

  def tire_sampleparams
    params.require(:tire_sample).permit()
  end

end
