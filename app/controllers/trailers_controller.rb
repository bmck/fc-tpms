# $Id$
# $(c)$

class TrailersController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

  before_action :find_trailer, except: [:index, :new, :create]
  before_action :authenticate_user!

  def index
    smart_listing_create partial: 'trailers/list'
  end

  def new
    @trailer = Trailer.new
  end

  def create
    @trailer = Trailer.create(trailer_params)
  end

  def edit
  end

  def update
    @trailer.update(trailer_params)
  end

  def destroy
    @trailer.destroy
  end

  def smart_listing_resource
    @trailer ||= params[:id] ? Trailer.find(params[:id]) : Trailer.new(params[:trailer])
  end
  helper_method :smart_listing_resource

  def smart_listing_collection
    scoped_trailers = \
    if current_user.global_admin?
      Trailer.all_trailers
    else
      Trailer.company_trailers(current_user.company_id)
    end

    scoped_trailers = scoped_trailers.contains(params[:filter]) if params[:filter]

    @trailers ||= scoped_trailers
  end
  helper_method :smart_listing_collection

  private

  def find_trailer
    @trailer = Trailer.find(params[:id])
  end

  def trailer_params
    params.require(:trailer).permit(:company_id, :trailer_serial)
  end
end
