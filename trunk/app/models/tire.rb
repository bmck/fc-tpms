# $Id$
# $(c)$

class Tire < ActiveRecord::Base
  has_paper_trail

  belongs_to :tire_type
  belongs_to :using_company
  belongs_to :owning_company
  belongs_to :sensor
  belongs_to :tire_location

  scope :all_tires, -> {}
  scope :company_tires, -> company_id { where("using_company_id = #{company_id} or " \
                                              "owning_company_id = #{company_id}") }

  scope :contains, -> x { where("locate(\"#{x}\", name) > 0") }

  delegate :trucks, :trailers, :in_storage, to: :tire_location

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
