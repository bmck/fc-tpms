# $Id: int_extensions.rb 5180 2015-01-03 22:03:32Z bmck $
# $(c): Copyright 2008-2015 by Capitalytics $

module IntExtensions
  extend ActiveSupport::Concern
  include ::NumericExtensions
  include ActionView::Helpers::NumberHelper

  def stringify_with_commas
    number_with_precision(
      self,
      delimiter: ',',
      significant: true,
      strip_insignificant_zeros: true,
      precision: to_s.length
    )
  end

  def stringify_as_bw_curr
    number_to_currency(
      self,
      delimiter: ',',
      negative_format: '(%u%n)'
    )
  end

  def to_pct_string
    number_to_percentage(
      to_pct,
      precision: to_s.length,
      significant: true,
      strip_insignificant_zeros: true
    )
  end
end
