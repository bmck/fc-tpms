# $Id$
# $(c)$

class TiresController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

  def index
    @tires = smart_listing_create partial: "tires/list"
  end

  def new
    @tire = Tire.new
  end

  def create
    @tire = TireType.create(tire_params)
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
  helper_method :smart_listing_resource

  def smart_listing_collection
    @tire_types ||= Tire.all_tires
  end
  helper_method :smart_listing_collection

  private

  def find_tire
    @tire = Tire.find(params[:id])
  end

  def tire_params
    params.require(:tire).permit(:sensor_id, :company_id, :tire_type_id)
  end
end
