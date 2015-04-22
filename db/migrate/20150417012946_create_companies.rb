# $Id$
# $(c)$

class CreateCompanies < ActiveRecord::Migration
  def change
    create_table :companies do |t|
      t.string      :name, limit: 64, null: false
      t.date        :start_service, null: false
      t.date        :end_service, null: false
      t.string      :contact_name, limit: 64, null: false
      t.string      :contact_address, limit: 128, null: false
      t.string      :contact_city, limit: 32, null: false
      t.string      :contact_state, limit: 2, null: false
      t.string      :contact_zip, limit: 10, null: false
      t.string      :contact_email, limit: 64, null: false
      t.string      :domain_name, limit: 128, null: false

      t.timestamps null: false
    end
  end
end
