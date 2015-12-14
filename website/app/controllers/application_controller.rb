# $Id$
# $(c)$

class ApplicationController < ActionController::Base
  helper :all # include all helpers, all the time

  # Prevent CSRF attacks by raising an exception.
  # For APIs, you may want to use :null_session instead.
  protect_from_forgery with: \
    (Proc.new{ |c| c.request.format == 'application/json' } ? :null_session : :exception)

  before_action \
    :assign_current_user,
    :authorize_action

  cattr_reader :screened_param_keys do
    %w(
      authenticity_token
      commit
      current_user
      request
      utf8
    )
  end

  rescue_from NewcoError::VerificationError do |exception|
    respond_to do |format|
      format.json { render json: exception.message.to_json, status: :forbidden }
      format.html { render_403(exception) }
    end
  end

  rescue_from NewcoError::AbstractMethodError do |exception|
    respond_to do |format|
      format.json { render json: exception.to_json, status: :forbidden }
      format.html { render_500(exception) }
    end
  end

  rescue_from CanCan::AccessDenied do |exception|
    respond_to do |format|
      format.json {
        puts "#{__FILE__}:#{__LINE__} -- #{exception.backtrace.join("\n")}"
        render json: exception.message.to_json, status: :forbidden
      }
      format.html {
        Rails.logger.debug "Access denied on #{exception.action} #{exception.subject.inspect}"
        Rails.logger.debug "\n\n#{caller.join("\n")}\n\n"
        render_550(exception)
      }
    end
  end

  rescue_from ActionView::MissingTemplate do |exception|
    respond_to do |format|
      format.json { render json: exception.message.to_json, status: :forbidden }
      format.html { render_406(exception) }
    end
  end

  rescue_from ActionController::UnpermittedParameters do |exception|
    Airbrake.notify(
      error_class: 'UnpermittedParameters',
      error_message: 'Unpermitted Parameters',
      parameters: { parameters: params, exception: exception }
    )
  end

  protected

  def rescue_action_in_public(exception)
    notify_airbrake exception
  end

  def authorize_action
    Rails.logger.debug "action = #{params[:action]}, controller = #{params[:controller]}"
    authorize!(params[:action].to_sym, params[:controller].to_sym)
  end

  alias_method :logged_in?, :current_user

  delegate :global_admin?, to: :current_user, allow_nil: true
  helper_method :logged_in?, :global_admin?

  def self.error_msg_prefix
    'ERROR: '
  end

  def assign_current_user
    Thread.current[:current_user] = current_user
  end

  private

  def render_403(exception)
    @exception = exception
    render status: 403 and return
  end

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
