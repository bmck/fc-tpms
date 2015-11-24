# $Id$
# $(c)$

module Api
  module V1
    class TireSerializer < ActiveModel::Serializer
      attributes :id, :tire_type

      def tire_type
        object.tire_type_name
      end
    end
  end
end
