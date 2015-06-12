# $Id$
# $(c)$

class CreateTireTypes < ActiveRecord::Migration
  def change
    create_table :tire_types do |t|
      t.string   :name, null: false
      t.boolean  :active, default: true, null: false

      t.timestamps null: false
    end
  end
end
