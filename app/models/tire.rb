# $Id$
# $(c)$

class Tire < ActiveRecord::Base
  has_paper_trail

  belongs_to :tire_type
  belongs_to :using_company
  belongs_to :owning_company
  belongs_to :sensor

  scope :all_tires, -> {}

  scope :contains, -> x { where("locate(\"#{x}\", name) > 0") }

  def name
    serial
  end

  def tire_type_name
    tire_type.name
  end

  def using_company_name
    using_company.name
  end

  def owning_company_name
    owning_company.name
  end

  def sensor_serial
    sensor.serial
  end
end
