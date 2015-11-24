# $Id$
# $(c)$

module Api
  module V1
    class TruckSerializer < ActiveModel::Serializer
      attributes :id, :name, :tires

      def name
        object.name
      end

      def tires
        ActiveModel::ArraySerializer.new(
          object.tires.to_a,
          each_serializer: Api::V1::TireSerializer
        )
      end
    end
  end
end
