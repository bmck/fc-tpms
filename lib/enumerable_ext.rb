# $Id$
# $(c)$
# from http://stackoverflow.com/questions/7749568/how-can-i-do-standard-deviation-in-ruby

module Enumerable

  alias_method :base_sum, :sum
  def sum
    select { |i| !i.nil? }.inject(0.0) { |accum, i| accum + i } rescue base_sum
  end

  def mean
    sum / length.to_f
  end

  def sample_variance
    m = mean
    sum = select { |i| !i.nil? }.inject(0.0) { |accum, i| accum + (i - m)**2 }
    sum / (length - 1).to_f
  end

  def standard_deviation
    Math.sqrt(sample_variance)
  end

end
