# $Id$
# $(c)$

module Api
  module V1
    class TireSerializer < ActiveModel::Serializer
      attributes :id, :tire_type, :last_sample_time, :last_psi,
        :hi_psi_threshold, :lo_psi_threshold, :time_threshold, :truck_id,
        :sensor_id

      def tire_type
        object.tire_type_name
      end

      def last_sample_time # seconds since Jan 1, 1970 (midnight GMT)
        object.sensor.tire_samples.sort { |y, x| y.sample_time <=> x.sample_time }.first.
          try(:sample_time).try(:to_i) || -1.0
      end

      def time_threshold # 3 days, should be overriden by min(tire, using co) || tiretype
        min_thresh = [
          object.time_threshold,
          object.using_company.time_threshold
        ].compact
        return min_thresh.min if min_thresh.length > 0

        object.tire_type.time_threshold || (3 * (60 * 60 * 24))
      end

      def last_psi
        object.sensor.tire_samples.sort { |y, x| y.sample_time <=> x.sample_time }.first
        .try(:psi) || -1.0
      end

      def hi_psi_threshold # 110% of nominal psi, should be overriden by tire, tiretype
        object.hi_psi_threshold ||
          object.tire_type.hi_psi_threshold ||
          object.tire_type.nominal_psi * 1.1
      end

      def lo_psi_threshold # 90% of nominal psi, should be overriden by tire, tiretype
        object.lo_psi_threshold ||
          object.tire_type.lo_psi_threshold ||
          object.tire_type.nominal_psi * 0.9
      end

      def truck_id
        object.tire_location_id
      end
    end
  end
end
