# $Id$
# $(c)$

class TireSample < ActiveRecord::Base
  has_paper_trail

  validates_uniqueness_of :sample_time, scope: :sensor_id, message: 'must be unique for a sensor'
  validates_presence_of :value, :sample_time, message: 'must be provided'
  validate :sensor_must_belong_to_tire

  belongs_to :sensor
  belongs_to :receiver

  scope :ordered_samples, -> { order('sample_time desc') }

  scope :all_samples, -> {
    active
    .joins(
      'LEFT JOIN (tires, sensors, receivers) ON (' +
      [
        'receivers.id = tire_samples.receiver_id',
        'sensors.id = tire_samples.sensor_id',
        'tires.sensor_id = tire_samples.sensor_id'
      ].join(' and ') +
      ')'
    )
  }
  scope :active, -> {
    where(active: true)
  }
  scope :company, -> company_id {
    where("#{company_id} = tires.using_company_id")
  }
  scope :sensor, -> sensor_id {
    where("#{sensor_id} = sensor_id")
  }
  scope :receiver, -> receiver_id {
    where("#{receiver_id} = receiver_id")
  }
  scope :contains, -> x {
    where(
      [
        "locate(\"#{x}\", tires.serial) > 0",
        "locate(\"#{x}\", receivers.serial) > 0",
        "locate(\"#{x}\", value) > 0",
        "locate(\"#{x}\", sample_time) > 0"
      ].join(' or ')
    )
  }
  scope :min_val, -> val {
    where("value >= #{val.to_f}")
  }
  scope :max_val, -> val {
    where("value <= #{val.to_f}")
  }

  delegate :tire, to: :sensor
  delegate :company, :tire_type, :company_id, to: :tire

  def company_name
    company.name
  end

  def tire_type_name
    tire_type.name
  end

  def tire_name
    tire.name
  end

  def sensor_name
    sensor.name
  end

  def receiver_name
    receiver.name
  end

  def destroy
    self.active = false
    save!
  end

  def sensor_must_belong_to_tire
    unless Sensor.find(sensor_id).try(:tire)
      errors.add(:sensor_id, 'must be associated with a valid tire')
    end
  end
end
