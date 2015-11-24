# $Id$
# $(c)$

module Api
  module V1
    class TiresController < BaseController
      before_action :authenticate_user!
      respond_to :json

      def index
        trucks = Truck.all_trucks
        trucks = trucks.company(current_user.company_id) unless current_user.global_admin?
        trucks &= params[:truck_id] if params[:truck_id]

        tires = trucks.map { |t| t.tires }.flatten
        tires &= params[:tire_id] if params[:tire_id]

        render json: ActiveModel::ArraySerializer.new(
          tires,
          each_serializer: Api::V1::TimeSerializer
        )
      end
    end
  end
end
