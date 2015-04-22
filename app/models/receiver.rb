# $Id$
# $(c)$

class Receiver < ActiveRecord::Base

  scope :all_receivers, -> {}

  def name
    serial
  end
end
