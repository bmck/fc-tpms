# $Id$
# $(c)$

class ApplicationController < ActionController::Base
  helper :all # include all helpers, all the time

  # Prevent CSRF attacks by raising an exception.
  # For APIs, you may want to use :null_session instead.
  protect_from_forgery with: :exception

  before_action \
    :reset_flash,
    :capture_orig_request_params, :log_action,
    :permitted_params,
    :authorize_action

  after_action :cache_request_data_in_flash

  # attr_reader :current_user

  def permitted_params
    params.except!(ApplicationController.screened_param_keys)
    @permitted_params ||= ::PermittedParams.new(params, current_user)
  end

  def self.screened_param_keys
    NON_PROPAGATED_PARAMS
  end

  NON_PROPAGATED_PARAMS = %w(
    authenticity_token
    commit
    current_layout
    current_user
    request
    utf8
  ) unless ApplicationController.const_defined?(:NON_PROPAGATED_PARAMS)

  rescue_from CanCan::AccessDenied do |exception|
    Rails.logger.debug "Access denied on #{exception.action} #{exception.subject.inspect}"
    Rails.logger.debug "\n\n#{caller.join("\n")}\n\n"
    render_550(exception)
  end

  rescue_from ActionView::MissingTemplate do |exception|
    render_406(exception)
  end

  rescue_from ActionController::UnpermittedParameters do |exception|
    Airbrake.notify(
      error_class: 'UnpermittedParameters',
      error_message: 'Unpermitted Parameters',
      parameters: { parameters: params, exception: exception }
    )
  end

  # def after_sign_in_path_for(resource)
  #   sign_in_url = new_user_session_url
  #   if request.referer == sign_in_url
  #     super
  #   else
  #     stored_location_for(resource) || '/home' || root_path
  #   end
  # end

  # def after_sign_out_path_for(resource)
  #   '/'
  # end

  private

  def render_404(exception)
    @exception = exception
    render template: 'errors/not_found', formats: [:html], status: 404 and return
  end

  def render_406(exception)
    !Collector.new(collect_mimes_from_class_level).negotiate_format(request)
    @exception = exception
    render template: 'errors/internal_server_error', formats: [:html], status: 406 and return
    fail
  end

  def render_422(exception)
    @exception = exception
    render template: 'errors/unprocessable_entity', formats: [:html], status: 422 and return
    fail
  end

  def render_500(exception)
    @exception = exception
    render template: 'errors/internal_server_error', formats: [:html], status: 500 and return
    fail
  end

  def render_550(exception)
    @exception = exception
    render template: 'errors/permission_denied', formats: [:html], status: 550 and return
  end

  protected

  def rescue_action_in_public(exception)
    notify_airbrake exception
  end

  def reset_flash
    flash.clear
  end

  # def fetch_logged_in_user
  #   return unless session[:user_id] && session[:user_type]
  #   @current_user = session[:user_type].safe_constantize.try(:find_by_id, session[:user_id])
  # end

  # def login_required
  #   return true if logged_in?
  #   session[:return_to] = request.url
  #   redirect_to controller: :login and return false
  # end

  # def tos_agreement_required
  #   return true if @current_user.agreed_to_tos?
  #   session[:return_to] = request.url
  #   flash.notice = 'You must agree to the Terms of Service before you may continue.'
  #   redirect_to controller: :login, action: :tos and return false
  # end

  def capture_orig_request_params
    if params[:attr] && !(params[:attr].try(:key?, :id).in?([false, nil])) && params[:attr][:id].is_a?(Array)
      params[:attr][:id] = params[:attr][:id] - ['']
      params[:attr][:id] = params[:attr][:id].first if params[:attr][:id].length == 1
    end

    @orig_request_params = params.deep_dup
  end

  def cache_request_data_in_flash
    flash[:pvs_url] = request.url
    # Caching plaintext passwords in a semi-permanent data structure is a security risk.
    if @orig_request_params.key?(:user)
      @orig_request_params[:user][:password] = '(hidden)' if @orig_request_params[:user].try(:key?, :password)
      @orig_request_params[:user][:password_confirmation] = '(hidden)' if
      @orig_request_params[:user].try(:key?, :password_confirmation)
    end

    @orig_request_params[:email].delete(:chart_img) if @orig_request_params.key?(:email) &&
      @orig_request_params[:email].is_a?(Hash)
    @orig_request_params.delete(:rates)
    @orig_request_params.delete(:net_volumes)
    @orig_request_params.delete(:orig_rates)
    @orig_request_params.delete(:prepayments)
    flash[:pvs_params] = @orig_request_params
  end

  def authorize_action
    # authorize!(params[:action].to_sym, params[:controller].to_sym)
  end

  alias_method :logged_in?, :current_user

  delegate \
    :global_admin?, to: :current_user, allow_nil: true

  helper_method \
    :logged_in?, :global_admin?

  def requires_global_admin
    user_allowed(global_admin?)
  end

  def self.error_msg_prefix
    'ERROR: '
  end

  def handle_controller_error(msg)
    flash.notice = ApplicationController.error_msg_prefix + msg
    begin
      redirect_to :back
    rescue
      redirect_to controller: :main
    end
  end

  def handle_caplyt_exceptions
    yield
  rescue RuntimeError => e
    Rails.logger.error { e.inspect }
    Rails.logger.error { ApplicationController.error_msg_prefix + e.msg }
    Rails.logger.error e.message
    Rails.logger.error e.backtrace.join("\n")
    handle_controller_error(e.msg) and return
  rescue SystemStackError => e
    Rails.logger.error e.message
    Rails.logger.error e.backtrace.join("\n")
  end

  def log_action
    # params[:controller] ||= controller_name
    # params[:action] ||= action_name
    # session_user_id = session.try(:[], :user_id).try(:to_i) || -1

    # entry = LogEntry.new
    # entry.timestamp = DateTime.now
    # entry.user_id = session_user_id
    # entry.session_id = request.session_options[:id]
    # entry.user_agent = request.env['HTTP_USER_AGENT']
    # entry.ip_address = request.env['REMOTE_ADDR']
    # entry.req_method = request.method
    # entry.full_url = "#{request.protocol}#{request.host_with_port}#{request.fullpath}"
    # entry.controller = controller_name
    # entry.action = action_name

    # # create a deep copy of the params hash
    # log_params = params.deep_dup.except(*screened_param_keys)
    # log_params[:email].try(:delete, :chart_img) # if log_params.key?(:email) && log_params[:email].is_a?(Hash)
    # hide_sensitive_params(log_params)
    # entry.params = log_params.inspect.encode('UTF-8')
    # entry.save!
  end

  private

  # recursive function that hides sensitive params prior to logging
  def hide_sensitive_params(params_hash, hide_keys = %w(password password_confirmation))
    return if params_hash.nil?

    params_hash.each_key do |key|
      if params_hash[key].is_a?(Hash)
        hide_sensitive_params(params_hash[:key], hide_keys)
      elsif hide_keys.include?(key)
        params_hash[key] = '(hidden)'
      end
    end
  end
end
