# $Id: tire_sample.rb 70 2015-04-27 04:11:23Z bmck_newco $
# $(c): Copyright 2015 by Newco $

class TireLocation < ActiveRecord::Base
  belongs_to :company
  has_many :tires

  scope :trucks, -> { where(type: 'Truck') }
  scope :trailers, -> { where(type: 'Trailer') }
  scope :in_storage, -> { where(type: 'Storage') }

  scope :all_locations, -> {}

  scope :company_locations, -> company_id { where(company_id: company_id) }
  scope :company_trucks, -> company_id { trucks.company_locations(company_id) }
  scope :company_trailers, -> company_id { trailers.company_locations(company_id) }
  scope :company_storage, -> company_id { storage.company_locations(company_id) }

  before_validation :assign_location_name, on: [:create, :update]

  def name
    fail NewcoError::AbstractMethodError.new
  end

  def company_name
    company.name
  end

  def assign_location_name
    self.location_name = name
  end
end
