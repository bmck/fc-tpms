# $Id$
# $(c)$

class TireController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

  def index
    @tires = smart_listing_create(:tires,
                                  TireType.all_tires,
                                  partial: 'tires/list',
                                  default_sort: { sensor_id: 'asc' })
  end

  def new
    @tire = TireType.new
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

  private

  def find_tire
    @tire = TireType.find(params[:id])
  end

  def tire_params
    params.require(:tire).permit(:name)
  end
end
