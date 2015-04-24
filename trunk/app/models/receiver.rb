# $Id$
# $(c)$

class Receiver < ActiveRecord::Base
  include Loggable

  scope :all_receivers, -> {}

  scope :contains, -> x { where("locate(\"#{x}\", name) > 0") }

  def name
    serial
  end
end
