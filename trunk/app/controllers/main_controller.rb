# $Id$
# $(c)$

class MainController < ApplicationController

  before_action :authenticate_user!, except: [:index]

  def index
    reset_session
    flash.notice = "You have successfully logged out. #{flash.notice}"
  end

  def home
    flash.notice = "You are logged in. #{flash.notice}"
  end

end
