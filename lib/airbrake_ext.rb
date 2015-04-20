# $Id$
# $(c)$

module Airbrake

  class << self
    def report_notice_not_sent_for_configuration(notice)
      # write_verbose_log("
      #   \n\n\n\nNotice was not sent due to configuration: \
      #   \n  Environment Monitored? #{configuration.public?} \
      #   \n  API key set? #{configuration.configured?} \
      #   \n\n  Action: #{notice.action rescue 'None'} \
      #   \n  Component: #{notice.component rescue 'None'} \
      #   \n  Error Class: #{notice.error_class rescue 'N/A'} \
      #   \n  Environment: #{configuration.environment_name} \
      #   \n  Params: #{notice.params.inspect} \
      #   \n\n  File: #{notice.backtrace.lines[1].try(:file)} \
      #   \n  Line Number: #{notice.backtrace.lines[1].try(:number)} \
      #   \n\n Backtrace: #{notice.backtrace.lines.join("\n")} \
      #   \n\n\n\n
      #   ")
    end

    private

    def send_notice(notice)
      if configuration.configured? && configuration.public?
        if configuration.async?
          configuration.async.call(notice)
          nil # make sure we never set env["airbrake.error_id"] for async notices
        else
          sender.send_to_airbrake(notice)
        end
      else
        report_notice_not_sent_for_configuration(notice)
      end
    end
  end

end
