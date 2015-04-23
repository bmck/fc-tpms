# $Id$
# $(c)$

class Company < ActiveRecord::Base
  has_many :users

  scope :all_companies, -> {}
  scope :active, -> { where('(start_service is null or start_service <= NOW()) and ' \
                            '(end_service >= NOW() or end_service is null)') }
  scope :my_company, -> company_id { where(id: company_id) }

  scope :contains, -> x { where("locate(\"#{x}\", name) > 0") }

end
