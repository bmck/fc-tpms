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
  domain: 'fleetcents.com',
  user_name: 'bmck@fleetcents.com',
  password: 'w4r4w4r4W',
  authentication: :plain,
  enable_starttls_auto: true
}
