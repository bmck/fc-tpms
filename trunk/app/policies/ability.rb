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

    if user.try(:global_admin?) == true
      can :manage, :all
      return
    end

    can [:new, :create], :"user/sessions"
    can [:new], :"user/password"
    can [:new, :create], :"user/registrations"
    can [:show], :"devise/confirmations"
    can :manage, :main

    return if user.nil?

    setup_abilities(user)
  end

  def setup_abilities(user)
    if user.company_admin?
      can :manage, :user, company: { id: user.company_id }
      can :manage, :trucks, company: { id: truck.company_id }
      can :manage, :trailers, company: { id: trailer.company_id }
      can :manage, :storages, company: { id: storage.company_id }
      can :manage, :tire_samples, company: { id: tire_sample.company_id }
    end

    setup_basic_user_abilities(user)
  end

  def setup_basic_user_abilities(_user)
    can [:destroy], :"user/sessions"
    can [:edit], :'user/registrations'

    can :manage, :tires, company: { id: tire.using_company_id }
    can :read, :tire_samples, company: { id: tire_sample.company_id }
    can :read, :trailers, company: { id: trailer.company_id }
    can :read, :trucks, company: { id: truck.company_id }
    can :read, :storages, company: { id: storage.company_id }
    can :read, :tire_sample_report
    can :new, :tire_types

    can :main, :all
  end
end
