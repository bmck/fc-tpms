# $Id$
# $(c)$

class MainController < ApplicationController
  respond_to :html, :js

  def index
    if user_signed_in?
      flash.notice = "You are logged in, #{current_user.first_last}. #{flash.notice}" unless \
        "You are logged in".in?(flash.notice.to_s)
    else
      render action: :home
    end
  end

  def home
  end
end
