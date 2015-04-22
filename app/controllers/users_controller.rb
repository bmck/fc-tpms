# $Id$
# $(c)$

class UsersController < ApplicationController
  include SmartListing::Helper::ControllerExtensions
  helper  SmartListing::Helper

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
    smart_listing_resource.update_attributes(user_params)
  end

  def destroy
    smart_listing_resource.destroy
  end

  def smart_listing_resource
    @user ||= params[:id] ? User.find(params[:id]) : User.new(params[:user])
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

    @users = scoped_users
    Rails.logger.verbose { "@users = #{@users.inspect}" }
    @users
  end

  private

  def find_user
    @user = User.find(params[:id])
  end

  def user_params
    params.require(:user).permit(:first_name, :last_name, :company, :email,
                                 :session_id, :role, :password, :password_confirmation)
  end
end
