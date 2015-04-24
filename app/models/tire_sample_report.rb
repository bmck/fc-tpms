# $Id$
# $(c)$

class TireSampleReport
  extend ActiveModel::Naming
  include ActiveModel::Conversion
  include ActiveModel::AttributeMethods

  attr_accessor :sensors, :receivers, :start_service, :end_service, :samples

  def initialize(params)
    sensor_ids = Array(params[:sensor_ids] || [])
    sensor_ids.select! { |sensor_id| !sensor_id.blank? && !Sensor.find(sensor_id).try(:tire).nil? }
    @sensors = sensor_ids.empty? ? Sensor.all_sensors.to_a :
      sensor_ids.map { |sensor_id| Sensor.find(sensor_id) }

    receiver_ids = Array(params[:receiver_ids])
    receiver_ids.select! { |receiver_id| !receiver_id.blank? && Receiver.find(receiver_id) }
    @receivers = receiver_ids.empty? ? Receiver.all.to_a :
      receiver_ids.map { |receiver_id| Receiver.find(receiver_id) }

    @start_service = Date.new(
      params[:"start_service(1i)"],
      params[:"start_service(2i)"],
      params[:"start_service(3i)"]
    ) rescue Date.today - 1.year

    @end_service = Date.new(
      params[:"end_service(1i)"],
      params[:"end_service(2i)"],
      params[:"end_service(3i)"]
    ) rescue Date.today
  end

  def tires
    (@sensors || []).map { |sensor| sensor.tire }
  end

  def samples_per_sensor
    @samples = {}
    condition_hsh = {}
    condition_hsh[:receiver_id] = receivers.map { |r| r.id } unless receivers.blank?
    condition_str = []
    condition_str << "sample_time <= STR_TO_DATE(\"#{end_service} 23:59:59\", GET_FORMAT(DATETIME,'ISO'))" unless end_service.nil?
    condition_str << "sample_time >= STR_TO_DATE(\"#{start_service} 00:00:00\", GET_FORMAT(DATETIME,'ISO'))" unless start_service.nil?
    sensors.each do |sensor|
      @samples[sensor] = TireSample.where(sensor_id: sensor.id)
      @samples[sensor] = @samples[sensor].where(condition_hsh) unless condition_hsh.keys.empty?
      @samples[sensor] = @samples[sensor].where(condition_str.join(' and ')) unless condition_str.empty?
      @samples[sensor] = @samples[sensor].order('sample_time asc').select('*')
      # Rails.logger.verbose { "sensor.id = #{sensor.id}" }
      # Rails.logger.verbose { "@samples[sensor] = #{@samples[sensor].to_sql}"}
      @samples[sensor] = @samples[sensor].to_a
    end
    # Rails.logger.verbose { "@samples = #{@samples.inspect}" }
    @samples
  end

end
