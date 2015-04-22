# $Id$
# $(c)$

class MainController < ApplicationController
  def index
    if user_signed_in?
      flash.notice = "You are logged in, #{current_user.first_last}. #{flash.notice}"
    else
      render action: :home
    end
  end

  def home
  end
end
