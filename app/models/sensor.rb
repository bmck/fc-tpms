# $Id$
# $(c)$

class Sensor < ActiveRecord::Base
  has_paper_trail

  validates_presence_of :sensor_type, :serial, message: 'must be provided'
  validates_uniqueness_of :serial, scope: :sensor_type

  has_one :tire

  scope :all_sensors, -> {
    joins('join tires on tires.sensor_id = sensors.id')
  }
  scope :company, -> company_id {
    where("tires.company_id = #{company_id}")
  }

  scope :contains, -> x {
    where("locate(\"#{x}\", sensors.serial) > 0")
  }

  def name
    serial
  end

  def tire_name
    tire.try(:name) || 'No Tire Known'
  end
end
