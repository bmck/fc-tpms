# $Id: kernel_ext.rb 5180 2015-01-03 22:03:32Z bmck $
# $(c): Copyright 2008-2015 by Capitalytics $

module Kernel
  def define_class(class_name_string, superclass_name_string = '::Object')
    n, ss = get_space_and_name(class_name_string)

    if superclass_name_string == '::Object'
      pn, pss = '::Object', Kernel
    else
      pn, pss = get_space_and_name(superclass_name_string)
    end

    ss.const_set(n, Class.new(pss.const_get(pn))) unless ss.constants.include?(n.to_sym)
    (k = ss.const_get(n)).class_exec { yield self if block_given? }
    # Rails.logger.verbose { "k = #{k.inspect}" }
    k
  end

  def exit_with_stdout_msg(msg, return_code)
    puts msg
    exit(return_code)
  end

  def exit_error(msg, error_code = 1)
    exit_with_stdout_msg("ERROR: #{msg}", error_code)
  end

  private

  def get_space_and_name(class_name_string)
    t = class_name_string.split('::') - ['']
    s, n = t[0..-2], t[-1]
    ss = s.join('::').safe_constantize || ::Object
    return n, ss
  end
end
