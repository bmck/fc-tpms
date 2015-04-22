# $Id$
# $(c)$

class CompaniesController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper
  helper_method :smart_listing_resource, :smart_listing_collection

  before_filter :smart_listing_resource, only: [:update, :destroy]

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
    @company.update_attributes(company_params)
  end

  def destroy
    @company.destroy
  end

  def smart_listing_resource
    @company ||= params[:id] ? Company.find(params[:id]) : Company.new(params[:company])
  end

  def smart_listing_collection
    scoped_companies = \
    if current_user.global_admin?
      Company.all_companies
    else
      Company.my_company
    end

    # Apply the search control filter.
    scoped_companies = scoped_users.contains(params[:filter]) if params[:filter]

    @companies ||= scoped_companies
  end

  private

  def find_company
    @company = Company.find(params[:id])
  end

  def company_params
    params.require(:company).permit(:company_name, :contact_name, :contact_address, :contact_phone,
                                    :contact_city, :contact_state, :contact_zip, :domain_name,
                                    :"start_service(1i)", :"start_service(2i)", :"start_service(3i)",
                                    :"end_service(1i)", :"end_service(2i)", :"end_service(3i)")
  end
end
