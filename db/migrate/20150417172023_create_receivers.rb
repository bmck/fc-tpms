# $Id$
# $(c)$

class CreateReceivers < ActiveRecord::Migration
  def change
    create_table :receivers do |t|
      t.string :serial, null: false
      t.string :receiver_type

      t.timestamps null: false
    end
  end
end
