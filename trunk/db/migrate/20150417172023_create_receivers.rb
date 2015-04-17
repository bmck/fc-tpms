# $Id: 20150416212650_create_tire_samples.rb 13 2015-04-17 02:03:45Z bmck_newco $
# $(c): Copyright 2015 by Newco $

class CreateReceivers < ActiveRecord::Migration
  def change
    create_table :receivers do |t|
      t.string :serial_no, null: false
      t.string :type

      t.timestamps null: false
    end
  end
end
