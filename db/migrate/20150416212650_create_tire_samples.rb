# $Id$
# $(c)$

class CreateTireSamples < ActiveRecord::Migration
  def change
    create_table :tire_samples do |t|
      t.integer  :tire_id, null: false
      t.integer  :receiver_id
      t.decimal  :sample, precision: 10, scale: 6, null: false
      t.datetime :sample_time

      t.timestamps null: false
    end
  end
end
