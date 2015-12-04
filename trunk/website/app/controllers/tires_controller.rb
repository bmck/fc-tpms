# $Id$
# $(c)$

class TiresController < ApplicationController
  respond_to :html, :js

  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

  before_action :find_tire, except: [:index, :new, :create]
  before_action :authenticate_user!

  def index
    respond_to do |format|
      format.any(:js, :html) {
        smart_listing_create partial: 'tires/list',
        sort_attributes: [[:name, :serial], [:sensor_name, "sensors.serial"],
                          [:tire_type_name, "tire_types.name"], [:owning_company_name, "owning_company.name"],
                          [:using_company_name, "using_company.name"],
                          [:tire_location, "tire_locations.location_name" ]]
      }
      format.json {
        trucks = Truck.all_trucks
        trucks = trucks.company(current_user.company_id) unless current_user.global_admin?
        trucks &= params[:truck_id] if params[:truck_id]

        tires = trucks.map { |t| t.tires }.flatten
        tires &= params[:tire_id] if params[:tire_id]

        render json: ActiveModel::ArraySerializer.new(
          tires,
          each_serializer: Api::V1::TimeSerializer,
          root: 'tires'
        ).to_json

      }
    end
  end

  def new
    @tire = Tire.new
  end

  def show
  end

  def create
    @tire = Tire.create(tire_params)
  end

  def edit
  end

  def update
    @tire.transaction do
      @tire.update(tire_params)
    end
  end

  def destroy
    @tire.destroy
  end

  def view_audit
    @tire.touch_with_version if @tire.versions.empty?
  end

  def smart_listing_resource
    @tire ||= params[:id] ? Tire.find(params[:id]) : Tire.new(params[:tire])
  end
  helper_method :smart_listing_resource

  def smart_listing_collection
    scoped_tires = Tire.all_tires

    scoped_tires = scoped_tires.company(current_user.company_id) unless \
      current_user.global_admin?

    scoped_tires = scoped_tires.contains(params[:filter]) unless \
      params[:filter].blank?

    @tires ||= scoped_tires
  end
  helper_method :smart_listing_collection

  private

  def find_tire
    @tire = Tire.find(params[:id])
  end

  def tire_params
    tp = params.require(:tire).permit(
      :sensor_serial, :sensor_id, :using_company_id, :owning_company_id,
      :tire_type_id, :tire_location_id, :serial, :location_notation
    )

    if tp[:sensor_serial]
      tp[:sensor_id] = Sensor.find_by_serial(tp[:sensor_serial]).try(:id)
      tp.delete(:sensor_serial)
    end

    tp
  end
end
