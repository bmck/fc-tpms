# $Id: tire_sample.rb 70 2015-04-27 04:11:23Z bmck_newco $
# $(c): Copyright 2015 by Newco $

class Trailer < TireLocation
  scope :all_trailers, -> { }
  scope :company_trailer, -> company_id { where(company_id: company_id) }

  def name
    "Trailer Number #{trailer_serial}"
  end
end
