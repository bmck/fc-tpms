# $Id$
# $(c)$

class AddTempColumn < ActiveRecord::Migration
  def change
    add_column :tire_samples, :tempc, :integer
  end
end
