# $Id: rails_config.rb 5180 2015-01-03 22:03:32Z bmck $
# $(c): Copyright 2008-2015 by Capitalytics $

if defined? RailsConfig
  RailsConfig.setup do |config|
    config.const_name = 'Settings'
  end
end
