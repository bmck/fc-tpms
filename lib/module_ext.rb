# $Id: module_ext.rb 5180 2015-01-03 22:03:32Z bmck $
# $(c): Copyright 2008-2015 by Capitalytics $

class Module
  def namespace
    name.split('::')
  end
end
