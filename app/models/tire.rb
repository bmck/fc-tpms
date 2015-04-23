# $Id$
# $(c)$

class Tire < ActiveRecord::Base
  belongs_to :tire_type
  belongs_to :company

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
end
