# $Id: tire_sample_report.rb 87 2015-04-30 02:20:03Z bmck_newco $
# $(c): Copyright 2015 by Newco $

module Report
  class PerStoragePieReport
    extend ActiveModel::Naming
    include ActiveModel::Conversion
    include ActiveModel::AttributeMethods
    include Chartable

    attr_accessor :company_id
    cattr_reader :title do 'Pct Tires by Type' end

    def initialize(params)
      @company_id = params[:company_id]
    end

    def data_name
      'category'
    end

    def value_name
      classname
    end

    def pie_chart_data
      #   [
      #   {
      #     "category": "category 1",
      #     "column-1": 8
      #   },
      #   {
      #     "category": "category 2",
      #     "column-1": 6
      #   },
      #   {
      #     "category": "category 3",
      #     "column-1": 2
      #   }
      # ]

      tires = (company_id.nil? ? Tire.all_tires : Tire.all_tires.company(company_id))

      tire_sql = tires.group('tire_location_id')
      .select("tire_locations.location_name as #{data_name}, count(*) as #{value_name}").to_sql

      rpt_data = ActiveRecord::Base.connection.select_all(tire_sql)

      serial_string = []
      rpt_data.rows.each do |rpt_dat|
        sample_string = []
        sample_string << "\"#{data_name}\" : \"#{rpt_dat[rpt_data.columns.index(data_name)]}\""
        sample_string << "\"#{value_name}\" : #{rpt_dat[rpt_data.columns.index(value_name)]}"
        sample_string = "{ #{sample_string.join(', ')} }"
        serial_string << sample_string
      end

      # Rails.logger.verbose { "serial_string = #{serial_string.inspect}" }

      '[' + serial_string.join(",\n") + ']'
    end

    def colors
      '[' + chart_colors(sensors.length).map { |col| "\"##{col}\"" }.join(",\n") + ']'
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
