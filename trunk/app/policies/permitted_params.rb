# $Id$
# $(c)$

# based on information presented at http://railscasts.com/episodes/371-strong-parameters
# Use this class to screen params provided in ctor, and to provide default values

Thread.exclusive do
  PermittedParamsStruct = Struct.new(:params, :user) unless defined? PermittedParamsStruct

  class PermittedParams < PermittedParamsStruct
    def initialize(p, u)
      pp = p.is_a?(ActionController::Parameters) ? p : ActionController::Parameters.new(p)
      # pp.merge!(
      #   current_user: u,
      #   current_user_type: u.class.name,
      #   current_user_id: u.id
      # ) if u

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
      recurse_superklass_methods(name)
    end

    def recurse_superklass_methods(methd)
      return ActionController::Parameters.new if methd.blank?

      klass = klass_name(methd)
      super_klass_methd = find_superklass_method(klass)
      return ActionController::Parameters.new if super_klass_methd.blank?

      send(super_klass_methd)
    end

    def object
      params.permit
      # :controller, :action, :current_user_id, :current_user_type, :current_user,
      # :user_validated, :email
    end

    private
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
