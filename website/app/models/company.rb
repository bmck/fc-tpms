# $Id$
# $(c)$

class Company < ActiveRecord::Base
  has_paper_trail

  has_many :users

  validates_presence_of :name, :contact_name, :contact_address, :contact_phone,
    :contact_city, :contact_state, :contact_zip,
    message: 'must be provided'
  validates_date :start_service, :end_service
  validates_date :end_service, :after => :start_service, :on_or_before => :today

  scope :all_companies, -> {
    active
  }
  scope :active, -> {
    where(active: true)
  }
  scope :servicing, -> {
    where('(start_service is null or start_service <= NOW()) and ' \
          '(end_service >= NOW() or end_service is null)')
  }

  scope :my_company, -> company_id { where(id: company_id) }

  scope :contains, -> x {
    where(
      [
        "locate(\"#{x}\", name) > 0",
        "locate(\"#{x}\", contact_name) > 0",
        "locate(\"#{x}\", contact_address) > 0",
        "locate(\"#{x}\", contact_city) > 0",
        "locate(\"#{x}\", contact_state) > 0",
        "locate(\"#{x}\", contact_zip) > 0",
        "locate(\"#{x}\", contact_email) > 0"
      ].join(' or ')
    )
  }

  def destroy
    self.active = false
    save!
  end
end
