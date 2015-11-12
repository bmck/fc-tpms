# $Id$
# $(c)$

# load all monkey patches
Dir[Rails.root.join('lib', '*.rb')].each do |f|
  require f
end

# load all environment/layout/controller settings
Dir[Rails.root.join('config', 'settings', '**', '*.yml')].each do |f|
  # Settings.add_source!(f)
end
# Settings.reload!
