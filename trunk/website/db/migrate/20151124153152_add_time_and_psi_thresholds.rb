# $Id$
# $(c)$

class AddTimeAndPsiThresholds < ActiveRecord::Migration
  def change
    add_column :tires, :time_threshold, :integer, default: nil
    add_column :tire_types, :time_threshold, :integer, default: nil
    add_column :users, :time_threshold, :integer, default: nil
    add_column :companies, :time_threshold, :integer, default: nil

    add_column :tires, :hi_psi_threshold, :float, default: nil
    add_column :tire_types, :hi_psi_threshold, :float, default: nil

    add_column :tires, :lo_psi_threshold, :float, default: nil
    add_column :tire_types, :lo_psi_threshold, :float, default: nil

    add_column :tire_types, :nominal_psi, :float, default: 0.0
  end
end
