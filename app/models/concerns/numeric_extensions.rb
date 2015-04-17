# $Id: numeric_extensions.rb 5180 2015-01-03 22:03:32Z bmck $
# $(c): Copyright 2008-2015 by Capitalytics $

module NumericExtensions
  extend ActiveSupport::Concern
  include ActionView::Helpers::NumberHelper

  # Round a number to the nearest value at the given magnitude.
  # E.g. at magnitude of 0.1, 4.5731 => 4.6.
  # E.g. at magnitude of 10000, 8234082.1234 => 8230000.
  def round_to_mag(mag)
    round(-Math.log10(mag).to_i)
  end

  def round_cents
    round(2)
  end

  def floor_n(mag)
    mag.zero? ? 0.0 : mag * (self / mag).to_i
  end

  def ceil_n(mag)
    floor_n(mag) == self ? self : floor_n(mag) + mag
  end

  def cond_abs(flag)
    flag ? abs : self
  end

  def resolution
    res = num_magnitude
    res /= 10.0 while round_to_mag(res) != self
    res
  end

  def num_magnitude
    zero? ? 0.0 : (10.0**(Math.log10(abs).floor))
  end

  def stringify_with_commas_parens(digits = nil)
    number_to_currency(
      self,
      delimiter: ',',
      format: '%n',
      negative_format: '(%n)',
      precision: (digits || 0),
      significant: false
    )
  end

  def stringify_with_commas(digits = nil)
    number_with_precision(
      self,
      delimiter: ',',
      significant: false,
      strip_insignificant_zeros: true,
      precision: (digits || 0)
    )
  end

  def stringify_as_bw_curr(digits = 2)
    number_to_currency(
      self,
      delimiter: ',',
      negative_format: '(%u%n)',
      precision: digits,
      significant: false
    )
  end

  def to_pct_string(digits = 2)
    number_to_percentage(
      to_pct,
      precision: digits,
      significant: false,
      strip_insignificant_zeros: true
    )
  end

  def to_pct
    self * 100
  end
end
