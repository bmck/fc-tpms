# $Id: tire_sample.rb 70 2015-04-27 04:11:23Z bmck_newco $
# $(c): Copyright 2015 by Newco $

class Trailer < TireLocation

  validates_presence_of :company_id, :trailer_serial, message: 'must be provided'
  validates_uniqueness_of :trailer_serial, scope: :company_id

  scope :all_trailers, -> {
    active
    .joins(
      'left join (companies) ' \
      'on (companies.id = company_id)'
    )
  }
  scope :active, -> {
    where(active: true)
  }
  scope :company, -> company_id { where(company_id: company_id) }

  scope :contains, -> x {
    where(
      [
        "locate(\"#{x}\", companies.name) > 0",
        "locate(\"#{x}\", trailer_serial) > 0"
      ].join(' or ')
    )
  }

  def name
    "Trailer #{trailer_serial} (#{company_name})"
  end

  def destroy
    self.active = false
    save!
  end
end
