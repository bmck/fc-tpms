# $Id$
# $(c)$

class StoragesController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

  before_action :find_storage, except: [:index, :new, :create]
  before_action :authenticate_user!

  def index
    smart_listing_create partial: 'storages/list'
  end

  def new
    @storage = Storage.new
  end

  def create
    @storage = Storage.create(storage_params)
  end

  def edit
  end

  def update
    @storage.update(storage_params)
  end

  def destroy
    @storage.destroy
  end

  def smart_listing_resource
    @storage ||= params[:id] ? Storage.find(params[:id]) : Storage.new(params[:storage])
  end
  helper_method :smart_listing_resource

  def smart_listing_collection
    scoped_storages = \
    if current_user.global_admin?
      Storage.all_storages
    else
      Storage.company_storage(current_user.company_id)
    end

    scoped_storages = scoped_storages.contains(params[:filter]) if params[:filter]

    @storages ||= scoped_storages
  end
  helper_method :smart_listing_collection

  private

  def find_storage
    @storage = Storage.find(params[:id])
  end

  def storage_params
    params.require(:storage).permit(:company_id, :storage_name, :storage_address, :storage_city, :storage_state, :in_storage_location)
  end
end
