# $Id$
# $(c)$

class CompanyController < ApplicationController
  before_action :authenticate_admin!

  def index
    @filterrific = initialize_filterrific(
      Company,
      params[:filterrific]
    ) or return
    @companies = @filterrific.find.page(params[:page])

    respond_to do |format|
      format.html
      format.js
    end
  end
end
