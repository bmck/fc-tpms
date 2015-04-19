# $Id$
# $(c)$

class CreateReceivers < ActiveRecord::Migration
  def change
    create_table :receivers do |t|
      t.string :serial_no, null: false
      t.string :type

      t.timestamps null: false
    end
  end
end
