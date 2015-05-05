# $Id: tire_sample.rb 70 2015-04-27 04:11:23Z bmck_newco $
# $(c): Copyright 2015 by Newco $

class Storage < TireLocation
  scope :all_storages, -> {
    joins(
      'left join (companies) ' \
      'on (companies.id = company_id)'
    )
  }
  scope :company, -> company_id {
    where(company_id: company_id)
  }

  scope :contains, -> x {
    where(
      [
        "locate(\"#{x}\", companies.name) > 0",
        "locate(\"#{x}\", storage_name) > 0",
        "locate(\"#{x}\", storage_address) > 0",
        "locate(\"#{x}\", storage_city) > 0",
        "locate(\"#{x}\", storage_state) > 0"
      ].join(' or ')
    )
  }

  def name
    "#{storage_name} Storage Facility (#{company_name})"
  end
end
