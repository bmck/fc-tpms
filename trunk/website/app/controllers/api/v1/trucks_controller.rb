# $Id$
# $(c)$

module Api
  module V1
    class TrucksController < BaseController
      prepend_before_action :authenticate_user!
      respond_to :json

      def index
        trucks = Truck.all_trucks
        trucks = trucks.company(current_user.company_id) unless current_user.global_admin?
        trucks &= params[:truck_id] if params[:truck_id]

        render json: ActiveModel::ArraySerializer.new(
          trucks,
          each_serializer: Api::V1::TruckSerializer
        )
      end
    end
  end
end
