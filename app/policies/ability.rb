# $Id$
# $(c)$

class Ability
  include CanCan::Ability if defined? CanCan

  def initialize(user)
    # Duplicated from https://github.com/CanCanCommunity/cancancan/wiki/Action-Aliases
    # so as to be clearly documented
    # clear_aliased_actions
    alias_action :index, :show, :view_audit, :to => :read
    # alias_action :new, :to => :create
    # alias_action :edit, :to => :update
    alias_action :update, :destroy, to: :modify
    # alias_action :show, :to => :read

    if user.try(:global_admin?) == true
      can :manage, :all
      return
    end

    can [:new, :create], :"user/sessions"
    can [:new, :edit], :"user/password"
    can [:new, :create, :edit, :update], :"devise/passwords"
    can [:new, :create], :"user/registrations"
    can [:new, :show, :create], :"devise/confirmations"
    can :manage, :main
    can [:create], :tire_samples

    return if user.nil?

    setup_abilities(user)
  end

  def setup_abilities(user)
    if user.company_admin?
      can :manage, User, company_id: user.company_id

      can :manage, Truck, company_id: user.company_id
      can :manage, Trailer, company_id: user.company_id
      can :manage, Storage, company_id: user.company_id

      can :create, TireType
      can :create, Tire
      can :destroy, Tire, owning_company_id: user.company_id

      can :create, TireSample
      can [:read, :update, :destroy], TireSample do |tire_sample|
        tire_sample.company_id == user.company_id
      end
    end

    setup_basic_user_abilities(user)
  end

  def setup_basic_user_abilities(user)
    can :main, :all

    can [:destroy], :"user/sessions"
    can [:edit], :'user/registrations'

    can :read, Trailer, company_id: user.company_id
    can :read, Truck, company_id: user.company_id
    can :read, Storage, company_id: user.company_id

    [
      Report::PerStoragePieReport,
      Report::PerTireSampleCountReport,
      Report::PerTireTypePieReport,
      Report::TireSampleCountReport,
      Report::TireSampleReport,
    ].each do |rpt_klass|
      can [:index, :create], rpt_klass
    end

    can [:read, :update], Tire, using_company_id: user.company_id

    can :read, TireSample do |tire_sample|
      tire_sample.company_id == user.company_id
    end
  end
end
