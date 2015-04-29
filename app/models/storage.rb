# $Id: tire_sample.rb 70 2015-04-27 04:11:23Z bmck_newco $
# $(c): Copyright 2015 by Newco $

class Storage < TireLocation

  scope :all_storages, -> { }
  scope :company_storage, -> company_id { where(company_id: company_id) }

  def name
    "Stored at #{storage_name} facility; in-storage location: #{in_storage_location}"
  end
end
