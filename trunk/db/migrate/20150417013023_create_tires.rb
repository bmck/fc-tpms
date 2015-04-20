# $Id$
# $(c)$

class CreateTires < ActiveRecord::Migration
  def change
    create_table :tires do |t|
      t.integer     :sensor_id, null: false
      t.integer     :tire_type_id
      t.integer     :company_id

      t.timestamps null: false
    end
  end
end
