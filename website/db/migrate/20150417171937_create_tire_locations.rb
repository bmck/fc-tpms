# $Id$
# $(c)$

class CreateTireLocations < ActiveRecord::Migration
  def change
    create_table :tire_locations do |t|
      t.string  :type,               null: false
      t.integer :company_id,         null: false
      t.string  :truck_serial
      t.string  :trailer_serial
      t.string  :storage_name
      t.string  :storage_address
      t.string  :storage_city
      t.string  :storage_state
      t.string  :location_name,     null: false
      t.boolean :active, default: true, null: false

      t.timestamps null: false
    end
  end
end
