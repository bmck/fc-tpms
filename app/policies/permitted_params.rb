# $Id$
# $(c)$

# based on information presented at http://railscasts.com/episodes/371-strong-parameters
# Use this class to screen params provided in ctor, and to provide default values

Thread.exclusive do
  PermittedParamsStruct = Struct.new(:params, :user) unless defined? PermittedParamsStruct

  class PermittedParams < PermittedParamsStruct
    def initialize(p, u)
      pp = p.is_a?(ActionController::Parameters) ? p : ActionController::Parameters.new(p)
      pp.merge!(
        current_user: u,
        current_user_type: u.class.name,
        current_user_id: u.id
      ) if u

      super(pp, u)
    end

    # helpers for traversing amalgamating class hierarchies

    def self.method_name(klass)
      return 'nil' if klass.nil?
      klass.name.underscore.gsub(/\//, '___').to_sym
    end

    def self.klass_name(methd)
      methd.to_s.gsub(/___/, '/').camelize.safe_constantize
    end

    def self.find_superklass_method(klass)
      superklass = klass.superclass
      return nil if superklass.nil?
      m = method_name(superklass)
      self.method_defined?(m) ? m : find_superklass_method(superklass)
    end

    delegate :method_name, :klass_name, :find_superklass_method, to: :class

    def method_missing(name, *_args)
      # Rails.logger.verbose { "method not found in PermittedParams: #{name}" }
      recurse_superklass_methods(name)
    end

    def recurse_superklass_methods(methd)
      # Rails.logger.verbose { "methd = #{methd}" }
      return ActionController::Parameters.new if methd.blank?

      klass = klass_name(methd)
      super_klass_methd = find_superklass_method(klass)
      return ActionController::Parameters.new if super_klass_methd.blank?

      send(super_klass_methd)
    end

    def object
      params.permit(
        :controller, :action, :current_layout, :current_user_id, :current_user_type, :current_user,
        :tag, :user_validated, :email
      )
    end

    def to_find_bucket_for_sim_params
      params.require(:bucket_id)
    end

    def to_create_transient_composite
      params.permit(:name, :field, :value, :root, :id, :bucket_id, :instrument_type, :instrument_field, :instrument_value)
    end

    def to_send_mail
      params.require(:email).permit(:pvs_url, :pvs_cgi, :message, :subject, :author_name, :author_addr)
    end

    def to_verify_otp
      params.require(:otp_val).to_i
    end

    private

    def force_geo_all!
      params.merge!(ActionController::Parameters.new(
                      geo: { geo_type: :all },
                      all: { id: 1 }
      ))
    end

    def merge_geo_all!
      force_geo_all! unless params.try(:[], :geo).try(:[], :geo_type)
    end

    def merge_latest_date!(klass)
      params[:date] = klass.select('distinct repdte').order('repdte desc').limit(1).to_a.first.repdte.to_s unless
      params.key?(:date)
    end

    def transform_params!
      # Rails.logger.verbose { "self = #{inspect}" }
      prefix = \
      case params[:action]
      when 'tabbed_key_performance' then 'tab'
      when 'colvis_key_performance' then 'colvis'
      when 'tabbed_colvis_key_performance' then 'tab_colvis'
      when 'bank_overview' then 'bov'
      when 'balance_sheet' then 'bsh'
      when 'income_statement' then 'inc'
      when 'exec_summary' then 'exec'
      when 'rank' then 'rank'
      when 'maturity' then 'mat'
      when 'principle_runoff' then 'pr'
      when 'loan_portfolio' then 'lp'
      when 'loan_detail' then 'ld'
      when 'cd_detail' then 'cd'
      when 'value_detail' then 'val'
      when 'low_spread_cd' then 'lscd'
      when 'avg_balances' then 'ab'
      when 'waived_fees' then 'wf'
      when 'report_set' then 'rpt'
      when 'nim_compare' then 'nimc'
      else
        return params
      end

      params[:geo] = {} unless params.key?(:geo)

      if params[:geo].key?("#{prefix}_geo_type") && !params[:geo].key?('geo_type')
        params[:geo]['geo_type'] = params[:geo]["#{prefix}_geo_type"].deep_dup
        params[:geo].delete("#{prefix}_geo_type".to_sym)
      end

      unless (pgt = params.try(:[], :geo).try(:[], :geo_type)).nil?
        if params[:geo].key?(pgt)
          params[pgt] = params[:geo][pgt].deep_dup
          params[:geo].delete(pgt)
        end

        if params[pgt].try(:key?, :attr) && !params.key?(:attr)
          params[:attr] = params[pgt][:attr].deep_dup
          params[pgt].delete(:attr)
        end

        if params[pgt].try(:[], "#{prefix}_id") && !params[pgt][:id]
          params[pgt][:id] = params[pgt]["#{prefix}_id"]
          params[pgt].delete("#{prefix}_id".to_sym)
        end
      end

      params
    end
  end

  Dir.glob(Rails.root.join('config', 'policies', 'permitted_params', '*.rb')).each do |params_file|
    require params_file.to_s
    module_name = Pathname.new(params_file).basename.sub_ext('').to_s
    module_suffix = module_name[-1] == 's' ? 's' : ''
    module_name = module_name.classify + module_suffix

    module_name = '::PermittedParams::' + module_name
    mod = module_name.safe_constantize
    next unless mod

    ::PermittedParams.send(:include, mod)
  end
end
