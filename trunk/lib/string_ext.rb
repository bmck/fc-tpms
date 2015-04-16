# $Id: string_ext.rb 5294 2015-01-23 15:59:21Z bmck $
# $(c): Copyright 2008-2015 by Capitalytics $

# Extend the core Ruby String class with Capitalytics-specific functionality.
class String

  def safe_to_date
    to_date
  rescue
    raise ::CapErr::InvalidDate.new
  end

  def safe_to_datetime
    to_datetime
  rescue
    raise ::CapErr::InvalidDateTime.new
  end

  def to_date_or_nil
    to_date
  rescue
    nil
  end

  def to_datetime_or_nil
    to_datetime
  rescue
    nil
  end

  def to_class
    self.split('::').inject(Object) do |mod, class_name|
      mod.const_get(class_name)
    end
  end

  def safer_constantize
    safe_constantize
  rescue NameError
    nil
  end
end
