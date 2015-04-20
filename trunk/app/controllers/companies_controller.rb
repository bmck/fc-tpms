# $Id$
# $(c)$

class CompaniesController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

  def index
    scoped_companies = \
    if current_user.global_admin?
      Company.all_companies
    else
      Company.my_company
    end

    # Apply the search control filter.
    scoped_companies = scoped_users.contains(params[:filter]) if params[:filter]

    @companies = smart_listing_create(:companies,
                                      scoped_companies,
                                      partial: 'companies/list',
                                      default_sort: { company_name: 'asc' })
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

  private

  def find_company
    @company = Company.find(params[:id])
  end

  def company_params
    params.require(:company).permit(:company_name, :contact_name, :contact_address, :contact_phone,
                                    :contact_city, :contact_state, :contact_zip)
  end
end
