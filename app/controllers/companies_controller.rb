# $Id$
# $(c)$

class CompaniesController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

  before_action :authenticate_user!

  def index
    @companies = smart_listing_create partial: 'companies/list'
  end

  def new
    @company = Company.new
  end

  def create
    @company = Company.create(company_params)
  end

  def edit
  end

  def update
    smart_listing_resource.update_attributes(company_params)
  end

  def destroy
    smart_listing_resource.destroy
  end

  def smart_listing_resource
    @company ||= params[:id] ? Company.find(params[:id]) : Company.new(params[:company])
  end
  helper_method :smart_listing_resource

  def smart_listing_collection
    scoped_companies = \
    if current_user.global_admin?
      Company.all_companies
    else
      Company.my_company
    end

    # Apply the search control filter.
    scoped_companies = scoped_users.contains(params[:filter]) if params[:filter]

    @companies = scoped_companies
  end
  helper_method :smart_listing_collection

  private

  def find_company
    @company = Company.find(params[:id])
  end

  def company_params
    params.require(:company).permit(:name, :contact_name, :contact_address, :contact_phone,
                                    :contact_city, :contact_state, :contact_zip, :domain_name,
                                    :"start_service(1i)", :"start_service(2i)", :"start_service(3i)",
                                    :"end_service(1i)", :"end_service(2i)", :"end_service(3i)",
                                    :contact_email)
  end
end
