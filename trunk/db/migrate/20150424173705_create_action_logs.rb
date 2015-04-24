# $Id$
# $(c)$

class CreateActionLogs < ActiveRecord::Migration
  def change
    create_table :action_logs do |t|
      t.string     :klass
      t.string     :action
      t.text       :old_state
      t.text       :new_state
      t.string     :user_email

      t.timestamps null: false
    end
  end
end
