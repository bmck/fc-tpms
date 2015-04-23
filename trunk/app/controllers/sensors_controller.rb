# $Id: sensors_controller.rb 38 2015-04-22 20:54:09Z bmck_newco $
# $(c): Copyright 2015 by Newco $

class SensorsController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

  before_action :authenticate_user!

  def index
    @sensors = smart_listing_create partial: 'sensors/list'
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
    smart_listing_resource.update_attributes(sensor_params)
  end

  def destroy
    smart_listing_resource.destroy
  end

  def smart_listing_resource
    @sensor ||= params[:id] ? Sensor.find(params[:id]) : Sensor.new(params[:sensor])
  end
  helper_method :smart_listing_resource

  def smart_listing_collection
    scoped_sensors = Sensor.all_sensors

    scoped_sensors = scoped_sensors.contains(params[:filter]) if params[:filter]

    @sensors = scoped_sensors
  end
  helper_method :smart_listing_collection

  private

  def find_sensor
    @sensor = Sensor.find(params[:id])
  end

  def sensor_params
    params.require(:sensor).permit(:type, :serial)
  end
end
class SensorsController < ApplicationController
end
