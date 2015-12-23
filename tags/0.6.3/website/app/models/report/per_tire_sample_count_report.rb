# $Id: tire_sample_report.rb 87 2015-04-30 02:20:03Z bmck_newco $
# $(c): Copyright 2015 by Newco $

module Report
  class PerTireSampleCountReport
    extend ActiveModel::Naming
    include ActiveModel::Conversion
    include ActiveModel::AttributeMethods
    include Chartable

    attr_accessor :start_service, :end_service, :num_samples, :num_tires
    cattr_reader :title do 'Avg Samples per Tire' end

    def initialize(params)
      company_id = params[:company_id]

      @start_service = Date.new(
        params[:"start_service(1i)"],
        params[:"start_service(2i)"],
        params[:"start_service(3i)"]
      ) rescue Date.today - 1.month

      @end_service = Date.new(
        params[:"end_service(1i)"],
        params[:"end_service(2i)"],
        params[:"end_service(3i)"]
      ) rescue Date.today

      every_tire = Tire.all.to_a
      @num_tires = (@start_service..@end_service).to_a.map! { |dt|
        every_tire.select { |tire| tire.version_at(dt.at_midday).try(:active) == true }.length
      }

      tire_samples = (company_id.nil? ? TireSample.all_samples :
                      TireSample.all_samples.company(company_id)).to_a
      @num_samples = (@start_service..@end_service).to_a.map! { |dt|
        tire_samples.select { |sample| sample.sample_time.to_date == dt }.length
      }

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

      '[' \
        '{' \
        '"id": "' + classname + '",' \
        '"valueField": "' + classname + '"' \
        '}' \
        ']'
    end

    def serial_data
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

      serial_string = []

      ((@start_service)..(@end_service)).to_a.each_with_index do |dt, ndex|
        sample_string = []
        tires, samples = @num_tires[ndex], @num_samples[ndex]
        ratio = tires.zero? ? 0.0 : samples.to_f / tires.to_f
        sample_string << "\"#{classname}\" : #{ratio}"
        sample_string << "\"#{serial_xaxis_name}\" : \"#{dt}\""
        sample_string = "{ #{sample_string.join(', ')} }"
        serial_string << sample_string
      end

      x = '[' + serial_string.join(",\n") + ']'
      # Rails.logger.verbose { "serial_string = #{x.inspect}" }

      # x
    end

    def subtitle
      "From #{start_service.to_s(:long)} thru #{end_service.to_s(:long)}"
    end

    def colors
      '[' + chart_colors(1).map { |col| "\"##{col}\"" }.join(",\n") + ']'
    end

    def serial_xaxis_name
      :sample_time
    end

    def yaxis_title
      'Samples per Tire / Day'
    end

    def filename
      classname
    end

    private

    def classname
      self.class.name.split('::').last.underscore
    end
  end
end
