# $Id: app.rb 5663 2015-04-02 14:09:56Z bmck $
# $(c): Copyright 2008-2015 by Capitalytics $

require 'rserve/simpler'

# per https://github.com/rails/activerecord-session_store/issues/6
ActiveRecord::SessionStore::Session.attr_accessible :data, :session_id

# load all monkey patches
Dir[Rails.root.join('lib', '*.rb')].each do |f|
  require f
end

# load all environment/layout/controller settings
Dir[Rails.root.join('config', 'settings', '**', '*.yml')].each do |f|
  Settings.add_source!(f)
end
Settings.reload!

# load all core attribute files
Dir[
  Rails.root.join('app', '**', 'attr_def.rb'),
  Rails.root.join('app', 'services', '**', '*.rb'),
  Rails.root.join('app', 'tenants', '**', '*.rb')
].each do |f|
  require f
end

[
  Geo::Types,
  Testbank::Geo::Types,
  Bxs::Geo::Types,
  CapErr,
  CapBug,
  PermittedParams,
  Ability
].each { |x| _ = x }

if Rails.env.production?
  Settings.layouts.to_hash.keys.each do |k|
    Dir[
      Rails.root.join('app', 'layouts', k.to_s, '**', '*.rb'),
      Rails.root.join('app', 'tenants', k.to_s, '**', '*.rb')
    ].each do |f|
      require f
    end
  end
end

Simulator::InstrumentDirectory.load
ApplicationController.check_prereqs
