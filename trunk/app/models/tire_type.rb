# $Id$
# $(c)$

class TireType < ActiveRecord::Base
  has_many :tires

  scope :all_tire_types, -> { }
  scope :this_company_tire_types -> company_id { joins("LEFT JOIN `tires` ON tires.tire_type_id = tire_types.id")
                                                 .where("tires.company_id = #{company_id}") }

end
