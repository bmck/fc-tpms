# $Id$
# $(c)$

class TrucksController < ApplicationController
  respond_to :html, :js

  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

  before_action :find_truck, except: [:index, :new, :create]
  before_action :authenticate_user!

  def index
    respond_to do |format|
      format.any(:js ,:html) {
        smart_listing_create partial: 'trucks/list',
        sort_attributes: [[:company_name, "companies.name"], [:serial, :truck_serial]]
      }
      format.json {
        trucks = Truck.all_trucks
        trucks = trucks.company(current_user.company_id) unless current_user.global_admin?
        trucks &= params[:truck_id] if params[:truck_id]

        render json: ActiveModel::ArraySerializer.new(
          trucks,
          each_serializer: Api::V1::TruckSerializer,
          root: 'trucks'
        ).to_json
      }
    end
  end

  def new
    @truck = Truck.new
  end

  def create
    @truck = Truck.create(truck_params)
  end

  def edit
  end

  def update
    @truck.update(truck_params)
  end

  def destroy
    @truck.destroy
  end

  def view_audit
    @truck.touch_with_version if @truck.versions.empty?
  end

  def smart_listing_resource
    @truck ||= params[:id] ? Truck.find(params[:id]) : Truck.new(params[:truck])
  end
  helper_method :smart_listing_resource

  def smart_listing_collection
    scoped_trucks = Truck.all_trucks

    scoped_trucks = scoped_trucks.company(current_user.company_id) unless current_user.global_admin?

    scoped_trucks = scoped_trucks.contains(params[:filter]) unless params[:filter].blank?

    @trucks ||= scoped_trucks
  end
  helper_method :smart_listing_collection

  private

  def find_truck
    @truck = Truck.find(params[:id])
  end

  def truck_params
    params.require(:truck).permit(:company_id, :truck_serial, :location_name)
  end
end
