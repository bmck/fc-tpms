# $Id$
# $(c)$

class Receiver < ActiveRecord::Base
  has_paper_trail

  scope :all_receivers, -> {}

  scope :contains, -> x { where("locate(\"#{x}\", serial) > 0") }

  def name
    serial
  end
end
