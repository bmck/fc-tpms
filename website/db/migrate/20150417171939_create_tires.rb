# $Id$
# $(c)$

class CreateTires < ActiveRecord::Migration
  def change
    create_table :tires do |t|
      t.integer     :sensor_id, null: false
      t.integer     :tire_type_id, null: false
      t.integer     :using_company_id, null: false
      t.integer     :owning_company_id, null: false
      t.integer     :tire_location_id, null: false
      t.string      :location_notation
      t.string      :serial
      t.boolean     :active, default: true, null: false

      t.timestamps null: false
    end
  end
end
