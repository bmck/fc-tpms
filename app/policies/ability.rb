# $Id$
# $(c)$

class Ability
  include CanCan::Ability if defined? CanCan

  cattr_accessor :layout

  def initialize(user)
    if user.try(:global_admin?) == true
      can :manage, :all
      return
    end

    can :manage, :testbank_reports

    cannot [
      :create_cash_flow_sim,
      :configure_cash_flow_sim,
      :configure_rates,
      :save_rates,
      :configure_policies,
      :save_pap,
      :configure_net_volumes,
      :save_net_volumes,
      :configure_prepayments,
      :save_prepayments,
      :configure_dep_distribs,
      :save_dep_distribs,
      :duplicate_cash_flow_sim,
      :schedule_cash_flow_sim,
      :unschedule_cash_flow_sim,
      :delete_cash_flow_sim
    ], :testbank_reports

    cannot [
      :add2bank,
      :convert,
      :configure_consultant,
      :update_consultant
    ], :users

    setup_controller_abilities(user) unless user.nil?
    setup_model_abilities(user)
  end

  def setup_controller_abilities(_user)
    true
  end

  def setup_model_abilities(user)
    setup_model_create_abilities(user)
    setup_model_read_abilities(user)
    setup_model_update_abilities(user)
    setup_model_delete_abilities(user)
  end

  def setup_model_create_abilities(_user)
    true
  end

  def setup_model_read_abilities(_user)
    true
  end

  def setup_model_update_abilities(_user)
    true
  end

  def setup_model_delete_abilities(_user)
    true
  end
end

Dir.glob(Rails.root.join('config', 'policies', 'ability', '*.rb')).each do |ability_file|
  require ability_file.to_s
  module_name = Pathname.new(ability_file).basename.sub_ext('').to_s
  module_suffix = module_name[-1] == 's' ? 's' : ''
  module_name = module_name.classify + module_suffix

  module_name = '::Ability::' + module_name
  mod = module_name.safe_constantize
  next unless mod

  Ability.send(:include, mod)
end
