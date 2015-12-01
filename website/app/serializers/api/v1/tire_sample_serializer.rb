# $Id$
# $(c)$

module Api
  module V1
    class TireSampleSerializer < ActiveModel::Serializer
      attributes :sample_time, :psi, :tire_id, :sensor_id, :receiver_id

      def tire_id
        object.sensor.tire.id
      end

      def psi
        object.psi.to_f
      end
    end
  end
end
