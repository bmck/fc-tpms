# $Id$
# $(c)$

class TireSample < ActiveRecord::Base
  has_paper_trail

  validates_presence_of :sensor_id, :receiver_id, :psi, :sample_time, message: 'must be provided'
  validates :psi, numericality: { greater_than_or_equal_to: 0.0, less_than: 9500.0 }
  validates :sensor_id, numericality: { only_integer: true, greater_than: 0 }
  validates :receiver_id, numericality: { only_integer: true, greater_than: 0 }
  validates_datetime :sample_time, on: :create, on_or_before: lambda { DateTime.now + 5.minutes },
    on_or_before_message: 'must not be in the far future' #, after: lambda { DateTime.now - 1.day }, after_message: 'tbd'
  validates_uniqueness_of :sample_time, scope: :sensor_id, message: 'must be unique for a sensor'
  validate :sensor_must_belong_to_tire
  validate :receiver_must_be_valid

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
        "locate(\"#{x}\", psi) > 0",
        "locate(\"#{x}\", sample_time) > 0"
      ].join(' or ')
    )
  }
  scope :min_psi, -> psi {
    where("psi >= #{psi.to_f}")
  }
  scope :max_psi, -> psi {
    where("psi <= #{psi.to_f}")
  }

  scope :min_tempc, -> tmpc {
    where("tempc is not null and tempc >= #{tmpc.to_f}")
  }
  scope :max_tempc, -> tmpc {
    where("tempc is not null and tempc <= #{tmpc.to_f}")
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

  def tempf
    return nil if tempc.nil?
    tempc * 1.8 + 32.0
  end

  def kpa
    psi * 0.145037738
  end

  def destroy
    self.active = false
    save!
  end

  def sensor_must_belong_to_tire
    unless Sensor.find(sensor_id).try(:tire)
      errors.add(:sensor_id, 'must be associated with a valid tire')
    end
  rescue ActiveRecord::RecordNotFound
    errors.add(:sensor_id, 'must be a valid sensor')
  end

  def receiver_must_be_valid
    Receiver.find(receiver_id)
  rescue ActiveRecord::RecordNotFound
    errors.add(:receiver_id, 'must be a valid receiver')
  end
end
