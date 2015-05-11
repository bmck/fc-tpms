# $Id: tire_sample_report.rb 87 2015-04-30 02:20:03Z bmck_newco $
# $(c): Copyright 2015 by Newco $

module Report
  class TireSampleCountReport
    extend ActiveModel::Naming
    include ActiveModel::Conversion
    include ActiveModel::AttributeMethods
    include Chartable

    attr_accessor :start_service, :end_service, :company_id
    cattr_reader :title do 'Tire Samples per Day' end

    def initialize(params)
      @company_id = params[:company_id]

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
    end

    def column_chart_graphs
      # /* [
      # /   {
      # /     "fillAlphas": 1,
      # /     "id": "AmGraph-1",
      # /     "title": "graph 1",
      # /     "type": "column",
      # /     "valueField": "column-1"
      # /   }
      # / ], */


      '[' \
        '{' \
        '"fillAlphas": 1,' \
        '"id": "' + classname + '-1",' \
        '"title": "Num Tire Samples per Day",' \
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

      dta_sql = (company_id.nil? ? TireSample.all_samples :
                 TireSample.all_samples.company(company_id))
      .where("date(sample_time) <= ? and date(sample_time) >= ?", end_service, start_service)
      .order('sample_time asc').group('date(sample_time)')
      .select("date(sample_time) as #{serial_xaxis_name}, count(*) as #{classname}").to_sql

      samples = ActiveRecord::Base.connection.select_all(dta_sql)
      Rails.logger.verbose { "samples = #{samples.inspect}" }

      sample_rows = samples.rows
      sample_row_dates = (@start_service..@end_service).to_a - sample_rows.map { |row| row[0] }
      sample_rows += sample_row_dates.map { |dt| [dt, 0.0] }
      sample_rows.sort! { |x, y| x[0] <=> y[0] }

      serial_string = []
      sample_rows.each do |sample|
        sample_string = []
        sample.each_with_index do |item, ndex|
          item = '"' + item.to_s + '"' if item.is_a?(Date) || item.is_a?(DateTime)
          sample_string << "\"#{samples.columns[ndex]}\" : #{item}"
        end
        sample_string = "{ #{sample_string.join(', ')} }"
        serial_string << sample_string
      end

      x = '[' + serial_string.join(",\n") + ']'
      Rails.logger.verbose { "serial_string = #{x.inspect}" }
      x
    end

    def data_date_format
      'YYYY-MM-DD'
    end

    def colors
      '[' + chart_colors(sensors.length).map { |col| "\"##{col}\"" }.join(",\n") + ']'
    end

    def serial_xaxis_name
      :sample_time
    end

    def yaxis_title
      'Tire Sample / Day'
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
