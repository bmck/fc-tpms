# $Id$
# $(c)$

module Report
  class TireSampleReport
    extend ActiveModel::Naming
    include ActiveModel::Conversion
    include ActiveModel::AttributeMethods
    include Chartable

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

    def line_chart_graphs
      #
      # [
      #   {
      #     "bullet": "round",
      #     "id": "AmGraph-1",
      #     "title": "graph 1",
      #     "valueField": "column-1"
      #   },
      #   {
      #     "bullet": "square",
      #     "id": "AmGraph-2",
      #     "title": "graph 2",
      #     "valueField": "column-2"
      #   }
      # ]

      num_graphs = sensors.length
      graph_bullets = chart_bullets(num_graphs)

      '[' +
      (0..(num_graphs - 1)).to_a.map { |sensor_ndex|
        sensor = sensors[sensor_ndex]
        '{' +
        '"bullet": "' + graph_bullets[sensor_ndex] + '",' \
        '"id": "' + sensor.name + '",' \
        '"title": "' + sensor.name + '",' \
        '"valueField": "' + sensor.name + '"' \
        '}'
      }.join(",\n") +
        ']'
    end

    def serial_data(chart_attr)
      # [
      #   {
      #     "column-1": 8,
      #     "column-2": 5,
      #     "date": "2014-03-01 07:57:57"
      #   },
      #   {
      #     "column-1": 6,
      #     "column-2": 7,
      #     "date": "2014-03-01 07:57:58"
      #   },
      #   {
      #     "column-1": 2,
      #     "column-2": 3,
      #     "date": "2014-03-01 07:57:59"
      #   },
      # ...
      # ]

      num_graphs, samples, serial_string = sensors.length, samples_per_sensor, []
      # Rails.logger.verbose { "samples.values = #{samples.values.inspect}" }
      sample_times = samples.values.map { |sensor_samples| sensor_samples.map { |ts| ts[:sample_time] } }.flatten.sort
      sensor_samples = {}
      sample_times.each { |sample_time| sensor_samples[sample_time] = { sample_time: sample_time.to_s(:db) } }

      num_graphs.times.each do |sensor_ndex|
        sensor = sensors[sensor_ndex]
        samples[sensor].each do |tire_sample|
          (sensor_samples[tire_sample[:sample_time]] ||= {})[sensor.name] =
            tire_sample.send(chart_attr.to_sym).to_f unless tire_sample.send(chart_attr.to_sym).nil?
        end
      end

      # Rails.logger.verbose { "sensor_samples = #{sensor_samples.inspect}" }

      sensor_samples = sensor_samples.values.map(&:to_s).join(",\n")
      .gsub(/:([^, ]*?)=\>/, '\1: ')
      .gsub(/([^, ]*?)=\>/, '\1: ')
      .gsub('sample_time', '"sample_time"')
      serial_string << sensor_samples

      # Rails.logger.verbose { "serial_string = #{serial_string.inspect}" }

      '[' + serial_string.join(",\n") + ']'
    end

    def colors
      '[' + chart_colors(sensors.length).map { |col| "\"##{col}\"" }.join(",\n") + ']'
    end

    def serial_xaxis_name
      :sample_time
    end

    def filename
      classname
    end

    def title(_arg)
      'Tire Samples'
    end

    def subtitle(arg)
      { psi: 'Tire Pressure [psi]',
        kpa: 'Tire Pressure [kPa]',
        tempf: 'Tire Temperature [degF]',
        tempc: 'Tire Temperature [degC]' }[arg]
    end

    alias_method :yaxis_title, :subtitle

    protected

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

    private

    def classname
      self.class.name.split('::').last.underscore
    end
  end
end
