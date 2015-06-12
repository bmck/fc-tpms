# $Id$
# $(c)$

module ActiveSupport
  class Logger
    def verbose(msg = nil, &block)
      msg ||= yield if block_given?
      debug "  AT #{caller[0].gsub(Rails.root.to_s + '/', '')} >>> #{msg}" # if Settings.try(:verbose) == true
    end

    def status(msg = nil, &block)
      msg ||= yield if block_given?
      callr = caller[1]
      info "[#{DateTime.now.strftime("%m/%y %H:%M:%S.%L")}] caller=#{callr} #{msg}"
    end

    def in_out(&block)
      callr = caller[1]
      # meth = callr.gsub(/^.*`/,'').gsub(/\'\s*$/,'')
      info "[#{DateTime.now.strftime("%m/%y %H:%M:%S.%L")}] Starting #{callr}"
      yield if block_given?
      info "[#{DateTime.now.strftime("%m/%y %H:%M:%S.%L")}] Finishing #{callr}"
    end
  end
end
