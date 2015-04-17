# $Id$
# $(c)$

class CreateCompanies < ActiveRecord::Migration
  def change
    create_table :companies do |t|
      t.string      :name, limit: 64, null: false
      t.datetime    :start_service, null: false
      t.datetime    :end_service, null: false

      t.timestamps null: false
    end
  end
end
