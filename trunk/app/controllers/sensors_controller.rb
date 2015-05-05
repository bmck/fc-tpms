# $Id$
# $(c)$

class SensorsController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

  before_action :find_sensor, except: [:index, :new, :create]
  before_action :authenticate_user!

  def index
    smart_listing_create partial: 'sensors/list'
  end

  def new
    @sensor = Sensor.new
  end

  def create
    @sensor = Sensor.create(sensor_params)
  end

  def edit
  end

  def update
    @sensor.update(sensor_params)
  end

  def destroy
    @sensor.destroy
  end

  def smart_listing_resource
    @sensor ||= params[:id] ? Sensor.find(params[:id]) : Sensor.new(params[:sensor])
  end
  helper_method :smart_listing_resource

  def smart_listing_collection
    scoped_sensors = Sensor.all_sensors

    scoped_sensors = scoped_sensors.company(current_user.company_id) unless \
      current_user.global_admin?

    scoped_sensors = scoped_sensors.contains(params[:filter]) unless params[:filter].blank?

    @sensors ||= scoped_sensors
  end
  helper_method :smart_listing_collection

  private

  def find_sensor
    @sensor = Sensor.find(params[:id])
  end

  def sensor_params
    params.require(:sensor).permit(:sensor_type, :serial)
  end
end
