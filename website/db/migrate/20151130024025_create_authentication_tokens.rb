# $Id$
# $(c)$

class CreateAuthenticationTokens < ActiveRecord::Migration
  def change
    create_table :authentication_tokens do |t|
      t.string :body
      # t.references :user, index: true, foreign_key: true
      t.integer :user_id, null: false
      t.datetime :last_used_at
      t.string :ip_address
      t.string :user_agent

      t.timestamps null: false
    end

    add_index :authentication_tokens, :user_id
  end
end
