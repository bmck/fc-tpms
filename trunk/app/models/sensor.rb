# $Id$
# $(c)$

class Sensor < ActiveRecord::Base
  belongs_to :tire

  scope :all_sensors, -> {}
  scope :all_company_sensors, -> company_id { joins("tires").where("tire.company_id = #{company_id}") }

  scope :contains, -> x { where("locate(\"#{x}\", name) > 0") }

  def name
    serial
  end
end
