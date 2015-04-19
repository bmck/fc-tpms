# $Id$
# $(c)$

class TireTypeController < ApplicationController
  before_action :authenticate_admin!

  def index
    @filterrific = initialize_filterrific(
      TireType,
      params[:filterrific]
    ) or return
    @tire_types = @filterrific.find.page(params[:page])

    respond_to do |format|
      format.html
      format.js
    end
  end
end
