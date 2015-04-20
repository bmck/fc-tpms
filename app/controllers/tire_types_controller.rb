# $Id$
# $(c)$

class TireTypeController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

  def index
    @tire_types = smart_listing_create(:tire_types,
                                       TireType.all_tire_types,
                                       partial: 'tire_types/list',
                                       default_sort: { name: 'asc' })
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

  private

  def find_tire_type
    @tire_type = TireType.find(params[:id])
  end

  def tire_type_params
    params.require(:tire_type).permit(:name)
  end
end
