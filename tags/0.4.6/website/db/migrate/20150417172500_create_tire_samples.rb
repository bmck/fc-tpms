# $Id$
# $(c)$

class CreateTireSamples < ActiveRecord::Migration
  def change
    create_table :tire_samples do |t|
      t.integer  :sensor_id, null: false
      t.integer  :receiver_id, null: false
      t.decimal  :value, precision: 10, scale: 6, null: false
      t.datetime :sample_time
      t.boolean  :active, default: true, null: false

      t.timestamps null: false
    end
  end
end
