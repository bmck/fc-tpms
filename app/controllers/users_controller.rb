# $Id$
# $(c)$

class UsersController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

  before_action :find_user, except: [:index, :new, :create]
  before_action :authenticate_user!

  def index
    smart_listing_create partial: 'users/list'
  end

  def new
    @user = User.new
  end

  def create
    @user = User.create(user_params)
  end

  def edit
  end

  def update
    u_parms = user_params.deep_dup
    u_parms.except!(:password, :password_confirmation) if u_parms[:password].blank? ||
      u_parms[:password_confirmation] != u_parms[:password]

    @user.update(u_parms)
  end

  def destroy
    @user.destroy
  end

  def smart_listing_resource
    @user ||= params[:id] ? User.find(params[:id]) : User.new(params[:user])
  end

  def smart_listing_collection
    scoped_users = User.all_users

    if current_user.company_admin?
      scoped_users = scoped_users.same_company(current_user.company_id)
    elsif current_user.basic_user?
      scoped_users = scoped_users.myself(current_user.id)
    end

    # Apply the search control filter.
    scoped_users = scoped_users.contains(params[:filter]) unless params[:filter].blank?

    @users ||= scoped_users
  end

  private

  def find_user
    @user = User.find(params[:id])
  end

  def user_params
    params.require(:user).permit(:first_name, :last_name, :company_id, :email,
                                 :session_id, :role, :password, :password_confirmation)
  end
end
