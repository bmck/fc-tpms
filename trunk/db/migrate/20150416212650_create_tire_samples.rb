# $Id$
# $(c)$

class CreateTireSamples < ActiveRecord::Migration
  def change
    create_table :tire_samples do |t|

      t.timestamps
    end
  end
end
