# $Id$
# $(c)$

class Ability
  include CanCan::Ability if defined? CanCan

  def initialize(user)
    # Duplicated from https://github.com/CanCanCommunity/cancancan/wiki/Action-Aliases
    # so as to be clearly documented
    # clear_aliased_actions
    # alias_action :index, :show, :to => :read
    # alias_action :new, :to => :create
    # alias_action :edit, :to => :update
    alias_action :update, :destroy, to: :modify
    # alias_action :show, :to => :read

    # Rails.logger.verbose do
    #   if user.nil?
    #     "I don\'t know who you are."
    #   else
    #     "You are #{user.first_last}, a #{user.role}."
    #   end
    # end

    if user.nil?
      can [:new, :create], :"devise/sessions"
      can :manage, :main
      return

    elsif user.global_admin?
      can :manage, :all
      return

    else
      setup_abilities(user)

    end
  end

  def setup_abilities(user)
    unless user.nil? # redundant
      if user.global_admin? # redundant
        can :manage, :all
      elsif user.company_admin?
        can :manage, :user, company: { id: user.company_id }
      elsif user.basic_user?
        setup_basic_user_abilities(user)
      end
    end

    true
  end

  def setup_basic_user_abilities(_user)
    can :manage, :tire
    can :manage, :tire_sample
    can :read, :tire_sample_report
    can :new, :tire_type

    can :main, :all
  end
end

# Dir.glob(Rails.root.join('config', 'policies', 'ability', '*.rb')).each do |ability_file|
#   require ability_file.to_s
#   module_name = Pathname.new(ability_file).basename.sub_ext('').to_s
#   module_suffix = module_name[-1] == 's' ? 's' : ''
#   module_name = module_name.classify + module_suffix

#   module_name = '::Ability::' + module_name
#   mod = module_name.safe_constantize
#   next unless mod

#   Ability.send(:include, mod)
# end
