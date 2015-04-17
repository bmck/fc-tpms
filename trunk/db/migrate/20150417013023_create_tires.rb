# $Id: app.rb 11 2015-04-16 21:45:33Z bmck_newco $
# $(c): Copyright 2015 by Newco $

class CreateTires < ActiveRecord::Migration
  def change
    create_table :tires do |t|

      t.timestamps
    end
  end
end
