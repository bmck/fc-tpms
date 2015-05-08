# $Id$
# $(c)$

class ReceiversController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

  before_action :find_receiver, except: [:index, :new, :create]
  before_action :authenticate_user!

  def index
    smart_listing_create partial: 'receivers/list'
  end

  def new
    @receiver = Receiver.new
  end

  def create
    @receiver = Receiver.create(receiver_params)
  end

  def edit
  end

  def update
    @receiver.update(receiver_params)
  end

  def destroy
    @receiver.destroy
  end

  def view_audit
  end

  def smart_listing_resource
    @receiver ||= params[:id] ? Receiver.find(params[:id]) : Receiver.new(params[:receiver])
  end
  helper_method :smart_listing_resource

  def smart_listing_collection
    scoped_receivers = Receiver.all_receivers

    scoped_receivers = scoped_receivers.contains(params[:filter]) unless params[:filter].blank?

    @receivers ||= scoped_receivers
  end
  helper_method :smart_listing_collection

  private

  def find_receiver
    @receiver = Receiver.find(params[:id])
  end

  def receiver_params
    params.require(:receiver).permit(:receiver_type, :serial)
  end
end
