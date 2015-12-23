# $Id$
# $(c)$

module Api
  module V1
    class TireSamplesController < ApplicationController
      before_action :authenticate_user!, :assign_current_user, :authorize_action
      respond_to :json

      def index
        trucks = Truck.all_trucks
        trucks = trucks.company(current_user.company_id) unless current_user.global_admin?
        trucks &= Array(params[:truck_id]).map(&:to_i).map { |tr| Truck.find(tr) } if params[:truck_id]

        tires = trucks.map { |t| t.tires }.flatten
        tires &= Array(params[:tire_id]).map(&:to_i).map { |ti| Tire.find(ti) } if params[:tire_id]

        tire_samples = tires.map { |t| t.sensor.tire_samples }.flatten
        tire_samples.sort! { |x, y| y.sample_time <=> x.sample_time }
        tire_samples = tire_samples[0..(Settings.max_tire_samples - 1)]

        render json: ActiveModel::ArraySerializer.new(
          tire_samples,
          each_serializer: Api::V1::TireSampleSerializer,
          root: "tire_samples"
        )
      end

      def create
        tire_sample = TireSample.create(tire_sample_params)
        if tire_sample.errors.any?
          render \
            text: (['Sample creation failed.'] +
                   Array(@tire_sample.errors.full_messages)).join('<br/>'+"\n"),
            status: 403 and return
        end

        render text: 'ok', status: 200 and return
      rescue ActiveRecord::RecordInvalid
        render text: 'fail', status: 403 and return
      end

      private

      def tire_sample_params
        begin
          params[:tire_sample][:psi] ||= params[:tire_sample][:value] if \
            params[:tire_sample][:value]
        rescue
        end

        begin
          params[:tire_sample][:tempc] ||= (params[:tire_sample][:tempf].to_f - 32.0) * 5.0/9.0 if \
            params[:tire_sample][:tempf].numeric?
        rescue
        end

        begin
          params[:tire_sample][:psi] ||= params[:tire_sample][:kpa].to_f * 0.145037738 if \
            params[:tire_sample][:kpa].numeric?
        rescue
        end

        params.require(:tire_sample).permit(:sensor_id, :receiver_id,
                                            :sample_time, :verify, :tempc, :psi)
      end


    end
  end
end
