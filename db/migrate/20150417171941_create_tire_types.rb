# $Id: 20150416212650_create_tire_samples.rb 13 2015-04-17 02:03:45Z bmck_newco $
# $(c): Copyright 2015 by Newco $

class CreateTireTypes < ActiveRecord::Migration
  def change
    create_table :tire_types do |t|
      t.string :name, null: false

      t.timestamps null: false
    end
  end
end
