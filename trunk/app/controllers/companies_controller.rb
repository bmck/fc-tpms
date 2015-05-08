# $Id$
# $(c)$

class CompaniesController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

  before_action :find_company, except: [:index, :new, :create]
  before_action :authenticate_user!

  def index
    smart_listing_create partial: 'companies/list'
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
    @company.update(company_params)
  end

  def destroy
    @company.destroy
  end

  def view_audit
  end

  def smart_listing_resource
    @company ||= params[:id] ? Company.find(params[:id]) : Company.new(params[:company])
  end
  helper_method :smart_listing_resource

  def smart_listing_collection
    scoped_companies = Company.all_companies

    scoped_companies = scoped_companies.my_company unless current_user.global_admin?

    scoped_companies = scoped_companies.contains(params[:filter]) unless params[:filter].blank?

    @companies ||= scoped_companies
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
