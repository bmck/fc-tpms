# $Id$
# $(c)$

class MainController < ApplicationController

  before_action :authenticate_user!

end
