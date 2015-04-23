# $Id$
# $(c)$

class TireSampleReport
  extend ActiveModel::Naming
  include ActiveModel::Conversion
  include ActiveModel::AttributeMethods

  attr_accessor :sensors, :receivers, :start_service, :end_service

  def initialize(params)
    @sensors = params[:sensors]
    @receivers = params[:receivers]
    @start_service = Date.new(
      params[:"start_service(1i)"],
      params[:"start_service(2i)"],
      params[:"start_service(3i)"]
    ) rescue Date.today - 1.year
    @end_service = Date.new(
      params[:"end_service(1i)"],
      params[:"end_service(2i)"],
      params[:"end_service(3i)"]
    ) rescue Date.today
  end

end
