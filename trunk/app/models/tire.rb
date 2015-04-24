# $Id$
# $(c)$

class Tire < ActiveRecord::Base
  include Loggable

  belongs_to :tire_type
  belongs_to :company
  belongs_to :sensor

  scope :all_tires, -> {}

  scope :contains, -> x { where("locate(\"#{x}\", name) > 0") }

  def name
    serial
  end

  def tire_type_name
    tire_type.name
  end

  def company_name
    company.name
  end

  def sensor_serial
    sensor.serial
  end
end
