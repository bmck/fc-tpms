# $Id$
# $(c)$

class ChgValueColumnToPsi < ActiveRecord::Migration
  def change
    rename_column :tire_samples, :value, :psi
  end
end
