# $Id: tire_sample.rb 70 2015-04-27 04:11:23Z bmck_newco $
# $(c): Copyright 2015 by Newco $

class Truck < TireLocation

  scope :all_trucks, -> { }
  scope :company_trucks, -> company_id { where(company_id: company_id) }

  def name
    "Truck Number #{truck_serial}"
  end
end
