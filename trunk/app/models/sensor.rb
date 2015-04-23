# $Id$
# $(c)$

class Sensor < ActiveRecord::Base
  has_one :tire

  scope :all_sensors, -> {}
  scope :all_company_sensors, -> company_id { joins("tires").where("tire.company_id = #{company_id}") }

  scope :contains, -> x { where("locate(\"#{x}\", name) > 0") }

  def name
    serial
  end

  def tire_name
    tire.name
  end
end
