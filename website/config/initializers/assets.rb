# $Id$
# $(c)$

# Be sure to restart your server when you modify this file.

# Version of your assets, change this if you want to expire all your assets.
Rails.application.config.assets.version = '1.00001'

# Precompile additional assets.
# application.js, application.css, and all non-JS/CSS in app/assets folder are already added.
# Rails.application.config.assets.precompile += %w( search.js )
# Rails.application.config.assets.precompile << %w(*.png *.jpg *.jpeg *.gif *.svg *.ico *.eot *.ttf *.wfoff)

Rails.application.config.assets.precompile << Proc.new do |path|
  if path =~ /\.(css|js|eot|svg|ttf|woff)\z/
    full_path = Rails.application.assets.resolve(path).to_path
    app_assets_path = Rails.root.join('app', 'assets').to_path
    full_path.starts_with? app_assets_path
    # if full_path.starts_with? app_assets_path
    #   puts "including asset: " + full_path
    #   true
    # else
    #   puts "excluding asset: " + full_path
    #   false
    # end
  else
    false
  end
end
