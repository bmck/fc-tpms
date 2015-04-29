# $Id$
# $(c)$

class TireSample < ActiveRecord::Base
  has_paper_trail

  belongs_to :sensor
  belongs_to :receiver

  scope :all_samples, -> {}
  scope :all_samples_for_company, -> company_id { joins("(sensors, tires) " \
                                                        "on tire_samples.sensor_id = sensors.id " \
                                                        "and sensors.tire_id = tires.id")
                                                  .where("#{company_id} == tires.company_id") }
  scope :all_samples_for_sensor, -> sensor_id { where("#{sensor_id} == sensor_id") }
  scope :all_samples_for_receiver, -> receiver_id { where("#{receiver_id} == receiver_id") }

  scope :contains, -> x { joins('LEFT JOIN (`tires`,`sensors`,`receivers`) ' \
                                'ON receivers.id = tire_samples.receiver_id and sensors.id = tire_samples.sensor_id and ' \
                                'tires.sensor_id = sensors.id')
                          .where("locate(\"#{x}\", tires.serial) > 0 or " \
                                 "locate(\"#{x}\", receivers.serial) > 0 or " \
                                 "locate(\"#{x}\", value) > 0 or " \
                                 "locate(\"#{x}\", sample_time) > 0") }

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

  # def to_h
  #   {
  #     id: id,
  #     sensor_id: sensor_id,
  #     receiver_id: receiver_id,
  #     value: value.to_f,
  #     sample_time: sample_time.to_s(:db),
  #     created_at: created_at,
  #     updated_at: updated_at
  #   }
  # end

  # alias_method :to_hash, :to_h
end
