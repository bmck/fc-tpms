# $Id: errors_controller.rb 5180 2015-01-03 22:03:32Z bmck $
# $(c): Copyright 2008-2015 by Capitalytics $

class ErrorsController < ApplicationController
  skip_before_action :login_required, :tos_agreement_required, :authorize_action

  ERRORS = [
    :internal_server_error, :not_found, :unprocessable_entity
  ].freeze

  ERRORS.each do |e|
    define_method e do
      respond_to do |format|
        format.html { render e, status: e }
        format.any { head e }
      end
    end
  end
end
