# $Id$
# $(c)$

module Api
  module V1
    class TrucksController < ApplicationController
      before_action :authenticate_user!, :assign_current_user, :authorize_action
      respond_to :json

      def index
        trucks = Truck.all_trucks
        trucks = trucks.company(current_user.company_id) unless current_user.global_admin?
        trucks &= Array(params[:truck_id]).map(&:to_i).map { |tr| Truck.find(tr) } if params[:truck_id]

        render json: ActiveModel::ArraySerializer.new(
          trucks,
          each_serializer: Api::V1::TruckSerializer,
          "trucks"
        )

      end
    end
  end
end
