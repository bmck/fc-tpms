# $Id$
# $(c)$

class Tire < ActiveRecord::Base
  belongs_to :tire_type
  belongs_to :company

  scope :all_tires, -> {}

  def tire_type_name
    tire_type.name
  end

  def name
    company.name
  end
end
