# $Id$
# $(c)$

module Api
  module V1
    class User::SessionsController < Devise::SessionsController
      prepend_before_filter :configure_permitted_parameters, only: :create
      # prepend_before_filter :allow_params_authentication!, only: :create
      respond_to :json

      # GET /resource/sign_in
      def new
        super
      end

      # POST /resource/sign_in
      def create
        token = Tiddle.create_and_return_token(current_user, request)
        render json: { authentication_token: token }
      end

      # DELETE /resource/sign_out
      def destroy
        # super
        Tiddle.expire_token(current_user, request) if current_user
        render json: {}
      end

      protected

      # You can put the params you want to permit in the empty array.
      def configure_permitted_parameters
        devise_parameter_sanitizer.for(:sign_in) << sign_in_params
      end

      private

      def sign_in_params
        params.require(:user).permit(:email, :password)
      end

      # this is invoked before destroy and we have to override it
      # def verify_signed_out_user
      # end

    end
  end
end
