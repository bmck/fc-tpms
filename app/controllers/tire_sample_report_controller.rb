# $Id$
# $(c)$

class TireSampleReportController < ApplicationController
  before_action :authenticate_user!

  def new
    @sensors = \
    if current_user.global_admin? || current_user.company_id.nil?
      Sensor.all_sensors
    else
      Sensor.all_company_sensors(current_user.company_id)
    end
    @receivers = Receiver.all_receivers
    @start_service = Date.today - 1.year
    @end_service = Date.today

    @object = TireSampleReport.new({ })
  end

  def create
    @report = TireSampleReport.new(report_params)
  end

  private

  def report_params
    params.require(:tire_sample_report).permit(:"start_service(1i)", :"start_service(2i)", :"start_service(3i)",
                                               :"end_service(1i)", :"end_service(2i)", :"end_service(3i)",
                                               sensor_ids: [], receiver_ids: [])
  end

end
