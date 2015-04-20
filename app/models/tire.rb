# $Id$
# $(c)$

class Tire < ActiveRecord::Base
  belongs_to :tire_type
  belongs_to :company

  scope :all_tires, -> {}
end
