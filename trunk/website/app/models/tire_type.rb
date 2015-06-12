# $Id$
# $(c)$

class TireType < ActiveRecord::Base
  has_paper_trail
  has_many :tires

  validates_presence_of :name, message: 'must be provided'
  validates_uniqueness_of :name

  scope :all_tire_types, -> {
    active
  }
  scope :active, -> {
    where(active: true)
  }
  scope :company, -> company_id {
    joins(
      'LEFT JOIN `tires` ON tires.tire_type_id = tire_types.id'
    ).where(
      "tires.company_id = #{company_id}"
    )
  }
  scope :contains, -> x {
    where("locate(\"#{x}\", name) > 0")
  }

  def destroy
    self.active = false
    save!
  end
end
