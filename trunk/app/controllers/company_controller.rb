# $Id$
# $(c)$

class CompanyController < ApplicationController

  before_action :authenticate_admin!

end
