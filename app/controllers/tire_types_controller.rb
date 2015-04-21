# $Id$
# $(c)$

class TireTypesController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

  def index
    @tire_types = smart_listing_create partial: "tire_types/list"
  end

  def new
    @tire_type = TireType.new
  end

  def create
    @tire_type = TireType.create(tire_type_params)
  end

  def edit
  end

  def update
    @tire_type.update_attributes(tire_type_params)
  end

  def destroy
    @tire_type.destroy
  end

  def smart_listing_resource
    @tire_type ||= params[:id] TireType.find(params[:id]) : TireType.new(params[:tire_type])
  end
  helper_method :smart_listing_resource

  def smart_listing_collection
    @tire_types ||= TireType.all_tire_types
  end
  helper_method :smart_listing_collection

  private

  def find_tire_type
    @tire_type = TireType.find(params[:id])
  end

  def tire_type_params
    params.require(:tire_type).permit(:name)
  end
end
