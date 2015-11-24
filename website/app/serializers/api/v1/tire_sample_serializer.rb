# $Id$
# $(c)$

module Api
  module V1
    class TireSampleSerializer < ActiveModel::Serializer
      attributes :sample_time, :psi, :tire_id

      def tire_id
        object.sensor.tire
      end
    end
  end
end
