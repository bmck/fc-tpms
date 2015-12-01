# $Id$
# $(c)$

module Api
  module V1
    class TiresController < ApplicationController
      before_action :authenticate_user!, :assign_current_user, :authorize_action
      respond_to :json

      def index
        trucks = Truck.all_trucks
        trucks = trucks.company(current_user.company_id) unless current_user.global_admin?
        trucks &= Array(params[:truck_id]).map(&:to_i).map{ |tr| Truck.find(tr) } if params[:truck_id]

        tires = trucks.map { |t| t.tires }.flatten
        tires &= Array(params[:tire_id]).map(&:to_i).map{ |ti| Tire.find(ti) } if params[:tire_id]

        render json: ActiveModel::ArraySerializer.new(
          tires,
          each_serializer: Api::V1::TireSerializer,
          "tires"
        )
      end
    end
  end
end
