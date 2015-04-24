# $Id$
# $(c)$

class Sensor < ActiveRecord::Base
  include Loggable

  has_one :tire

  scope :all_sensors, -> { joins('join tires on tires.sensor_id = sensors.id') }
  scope :all_company_sensors, -> company_id {
    joins("join tires on tires.sensor_id = sensors.id")
    .where("tires.company_id = #{company_id}")
  }

  scope :contains, -> x { where("locate(\"#{x}\", name) > 0") }

  def name
    serial
  end

  def tire_name
    tire.try(:name) || 'No Tire Known'
  end
end
