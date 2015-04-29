# $Id$
# $(c)$

class TiresController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

  before_action :find_tire, except: [:index, :new, :create]
  before_action :authenticate_user!

  def index
    smart_listing_create partial: 'tires/list'
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
    @tire.update(tire_params)
  end

  def destroy
    @tire.destroy
  end

  def smart_listing_resource
    @tire ||= params[:id] ? Tire.find(params[:id]) : Tire.new(params[:tire])
  end
  helper_method :smart_listing_resource

  def smart_listing_collection
    scoped_tires = \
    if current_user.global_admin?
      Tire.all_tires
    else
      Tire.company_tires(current_user.company_id)
    end

    scoped_tires = scoped_tires.contains(params[:filter]) if params[:filter]

    @tires ||= scoped_tires
  end
  helper_method :smart_listing_collection

  private

  def find_tire
    @tire = Tire.find(params[:id])
  end

  def tire_params
    params.require(:tire).permit(:sensor_id, :using_company_id, :owning_company_id, :tire_type_id, :location_notation)
  end
end
