# $Id$
# $(c)$

class TiresController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper
  helper_method :smart_listing_resource, :smart_listing_collection

  before_filter :smart_listing_resource, only: [:update, :destroy]

  def index
    @tires = smart_listing_create partial: "tires/list"
  end

  def new
    @tire = Tire.new
  end

  def create
    @tire = Tire.create(tire_params)
  end

  def edit
  end

  def update
    @tire.update_attributes(tire_params)
  end

  def destroy
    @tire.destroy
  end

  def smart_listing_resource
    @tire ||= params[:id] ? Tire.find(params[:id]) : Tire.new(params[:tire])
  end

  def smart_listing_collection
    scoped_tires = Tire.all_tires

    scoped_tires = scoped_tires.contains(params[:filter]) if params[:filter]

    @tires ||= scoped_tires
  end


  private

  def find_tire
    @tire = Tire.find(params[:id])
  end

  def tire_params
    params.require(:tire).permit(:sensor_id, :company_id, :tire_type_id)
  end

end
