# $Id$
# $(c)$

class TireSampleReportController < ApplicationController
  before_action :authenticate_user!

  def index
    @sensors = \
    if current_user.global_admin? || current_user.company_id.nil?
      Sensor.all_sensors
    else
      Sensor.all_company_sensors(current_user.company_id)
    end
    @receivers = Receiver.all_receivers
    @being_time_range = nil
    @end_time_range = nil
    @lo_val = nil
    @hi_val = nil
  end

  def new
    redirect_to :create and return
  end

  def create
  end
end
