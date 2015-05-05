# $Id: tire_sample.rb 70 2015-04-27 04:11:23Z bmck_newco $
# $(c): Copyright 2015 by Newco $

class Truck < TireLocation
  scope :all_trucks, -> {
    joins(
      'left join (companies) ' \
      'on (companies.id = company_id)'
    )
  }
  scope :company, -> company_id { where(company_id: company_id) }

  scope :contains, -> x {
    where(
      [
        "locate(\"#{x}\", companies.name) > 0",
        "locate(\"#{x}\", truck_serial) > 0"
      ].join(' or ')
    )
  }

  def name
    "Truck #{truck_serial} (#{company_name})"
  end
end
