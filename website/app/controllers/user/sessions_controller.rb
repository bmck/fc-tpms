class User::SessionsController < Devise::SessionsController
  before_filter :configure_sign_in_params, only: :create
  # prepend_before_filter :configure_permitted_parameters, only: :create

  respond_to :html, :json, :js

  # GET /resource/sign_in
  def new
    super
  end

  # POST /resource/sign_in
  def create
    respond_to do |format|
      format.any(:js, :html) {
        super
      }
      format.json {
        token = Tiddle.create_and_return_token(current_user, request)
        render json: { authentication_token: token } and return
      }
    end
  end

  # DELETE /resource/sign_out
  def destroy
    respond_to do |format|
      format.any(:js, :html) {
        super
      }
      format.json {
        Tiddle.expire_token(current_user, request) if current_user
        render json: {} and return
      }
    end
  ensure
    reset_session
    flash.notice = "You have successfully logged out. #{flash.notice}"
  end

  protected

  # You can put the params you want to permit in the empty array.
  def configure_sign_in_params
    devise_parameter_sanitizer.for(:sign_in) << sign_in_params
  end

  private

  def sign_in_params
    request.format == 'application/json' ?
      params.require(:user).permit(:email, :password) :
      params.permit()
  end
end
