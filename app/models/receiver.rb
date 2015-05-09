# $Id$
# $(c)$

class Receiver < ActiveRecord::Base
  has_paper_trail

  validates_presence_of :receiver_type, :serial, message: 'must be provided'
  validates_uniqueness_of :serial, scope: :receiver_type

  scope :all_receivers, -> {}

  scope :contains, -> x {
    where("locate(\"#{x}\", serial) > 0")
  }

  def name
    serial
  end
end
