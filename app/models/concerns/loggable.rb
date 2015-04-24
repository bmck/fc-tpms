# $Id$
# $(c)$

module Loggable
  extend ActiveSupport::Concern

  included do
    class_eval do
      attr_reader :old_state, :new_state

      before_update :set_old_state
      before_destroy :set_old_state
      after_commit :create_log_entry

      protected

      def mirror_action(action)
        {
          update: :update,
          create: :destroy,
          destroy: :create
        }[action]
      end

      private

      def set_old_state
        Rails.logger.verbose { "running #{__method__}" }
        @old_state = current_state
      end

      def set_new_state
        Rails.logger.verbose { "running #{__method__}" }
        @new_state = current_state
      end

      def create_log_entry
        Rails.logger.verbose { "running #{__method__}" }
        actn = \
        if old_state.nil? && !new_state.nil?
          :create
        elsif !old_state.nil? && new_state.nil?
          :destroy
        elsif !old_state.nil? && !new_state.nil?
          :update
        end

        parms = ActionController::Parameters.new()
        parms[:klass], parms[:action] = self.class.name, actn.to_s
        parms[:old_state], parms[:new_state] = old_state, new_state || current_state
        parms[:user_email] = Thread.current[:current_user].try(:email)

        ActionLog.create(parms.permit(:klass, :action, :old_state, :new_state, :user_email))
      end

      def current_state
        to_json
      end
    end
  end
end
