# $Id$
# $(c)$

class Tire < ActiveRecord::Base
  has_paper_trail

  belongs_to :tire_type
  belongs_to :using_company, class_name: 'Company'
  belongs_to :owning_company, class_name: 'Company'
  belongs_to :sensor
  belongs_to :tire_location

  validates_presence_of :using_company_id, :owning_company_id,
    :tire_type_id, :tire_location_id, :serial, message: 'must be provided'
  validate :tire_location_must_belong_to_using_company

  scope :all_tires, -> {
    active
    .joins(
      'left join (sensors, tire_types, companies as using_company, companies as owning_company, tire_locations) ' \
      'on (' +
      [
        'tires.sensor_id = sensors.id',
        'tires.tire_type_id = tire_types.id',
        'using_company.id = tires.using_company_id',
        'owning_company.id = tires.owning_company_id',
        'tires.tire_location_id = tire_locations.id'
      ].join(' and ') +
      ')'
    )
  }
  scope :active, -> {
    where(active: true)
  }

  scope :company, -> company_id {
    where(
      [
        "using_company_id = #{company_id}",
        "owning_company_id = #{company_id}"
      ].join(' or ')
    )
  }
  scope :contains, -> x {
    where(
      [
        "locate(\"#{x}\", tires.serial) > 0",
        "locate(\"#{x}\", sensors.serial) > 0",
        "locate(\"#{x}\", using_company.name) > 0",
        "locate(\"#{x}\", owning_company.name) > 0",
        "locate(\"#{x}\", tire_types.name) > 0"
      ].join(' or ')
    )
  }

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

  def location_name
    tire_location.name
  end

  def sensor_serial
    sensor.try(:serial) || ''
  end

  def tire_location_must_belong_to_using_company
    unless tire_location.company_id == using_company_id
      errors.add(:using_company, 'must provide the tire location')
    end
  end

  def destroy
    self.active = false
    save!
  end
end
