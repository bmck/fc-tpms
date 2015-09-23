# $Id$
# $(c)$

class CreateSensors < ActiveRecord::Migration
  def change
    create_table :sensors do |t|
      t.string  :serial, null: false
      t.string  :sensor_type
      t.boolean :active, default: true, null: false

      t.timestamps null: false
    end
  end
end
