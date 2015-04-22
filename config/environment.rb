# $Id$
# $(c)$

# Load the Rails application.
require File.expand_path('../application', __FILE__)

# Initialize the Rails application.
Rails.application.initialize!

ActionMailer::Base.raise_delivery_errors = true
ActionMailer::Base.perform_deliveries = true
ActionMailer::Base.delivery_method = :smtp
ActionMailer::Base.smtp_settings = {
  address: 'smtp.gmail.com',
  port: 587,
  domain: 'capitalytics.com',
  user_name: 'info@capitalytics.com',
  password: 'w4r4w4Rjp',
  authentication: :plain,
  enable_starttls_auto: true
}
