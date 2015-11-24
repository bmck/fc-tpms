# $Id$
# $(c)$

module Api
  module V1
    class BaseController < ApplicationController
      before_action :destroy_session
      skip_before_action :assign_current_user

      def destroy_session
        request.session_options[:skip] = true
      end

      # def assign_current_token_user
      #   if user_token_authenticable?
      #     @current_user ||= tokenized_user
      #   end
      #   assign_current_user
      # end

      protected

      def authenticate_user!
        user_email = tokenized_user_identifier
        user       = user_email && tokenized_user

        if user && Devise.secure_compare(user.authentication_token, provided_token)
          sign_in user, store: false
        end
      end

      def user_token_authenticable?
        return false unless request.format.json?
        return false if tokenized_user_identifier.blank?

        tokenized_user.try(:token_authenticable?) || false
      end

      private

      def tokenized_user
        User.find_by(email: tokenized_user_identifier.to_s)
      end

      def tokenized_user_identifier
        request.headers['X-User-Email'] || params[:user_email]
      end

      def provided_token
        request.headers['X-User-Token'] || params[:user_token]
      end
    end
  end
end
