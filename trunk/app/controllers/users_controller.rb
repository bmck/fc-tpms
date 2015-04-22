# $Id$
# $(c)$

class UsersController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper
  helper_method :smart_listing_resource, :smart_listing_collection

  before_filter :smart_listing_resource, only: [:update, :destroy]
  before_action :find_user, except: [:index, :new, :create]

  def index
    smart_listing_create :users,
      scoped_users,
      partial: 'users/list',
      default_sort: { last_name: 'asc', first_name: 'asc' }
  end

  def new
    @new_user = User.new
  end

  def create
    @user = User.create(user_params)
  end

  def edit
  end

  def update
    @user.update_attributes(user_params)
  end

  def destroy
    @user.destroy
  end

  def smart_listing_resource
    @tire ||= params[:id] ? User.find(params[:id]) : User.new(params[:user])
  end

  def smart_listing_collection
    scoped_users = \
    if current_user.global_admin?
      User.all_users
    elsif current_user.company_admin?
      User.same_company(current_user.company_id)
    else
      User.myself(current_user.id)
    end

    # Apply the search control filter.
    scoped_users = scoped_users.contains(params[:filter]) if params[:filter]

    @tire_types ||= scoped_users
  end

  private

  def find_user
    @user = User.find(params[:id])
  end

  def user_params
    params.require(:user).permit(:first_name, :last_name, :company, :email,
                                 :session_id)
  end
end
