# $Id$
# $(c)$

class CreateUsers < ActiveRecord::Migration
  def change
    create_table :users do |t|
      t.string :first_name, null: false
      t.string :last_name, null: false
      t.string :email, null: false
      t.string :role, null: false, default: :basic_user
      t.integer :company_id, default: nil

      t.timestamps null: false
    end
  end
end
