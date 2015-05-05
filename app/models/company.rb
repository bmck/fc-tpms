# $Id$
# $(c)$

class Company < ActiveRecord::Base
  has_paper_trail

  has_many :users

  scope :all_companies, -> {}
  scope :active, -> { where('(start_service is null or start_service <= NOW()) and ' \
                            '(end_service >= NOW() or end_service is null)') }
  scope :my_company, -> company_id { where(id: company_id) }

  scope :contains, -> x { where(
                            [
                              "locate(\"#{x}\", name) > 0",
                              "locate(\"#{x}\", contact_name) > 0",
                              "locate(\"#{x}\", contact_address) > 0",
                              "locate(\"#{x}\", contact_city) > 0",
                              "locate(\"#{x}\", contact_state) > 0",
                              "locate(\"#{x}\", contact_zip) > 0",
                              "locate(\"#{x}\", contact_email) > 0"
                            ].join(' or ')
  ) }
end
