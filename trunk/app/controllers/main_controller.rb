# $Id$
# $(c)$

class MainController < ApplicationController
  def index
    if user_signed_in?
      flash.notice = "You are logged in, #{current_user.first_last}. #{flash.notice}"
    else
      redirect_to '/home' and return
    end
  end

  def home
    reset_session
    flash.notice = "You have successfully logged out. #{flash.notice}"
  end
end
