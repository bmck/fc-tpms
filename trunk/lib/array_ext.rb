# $Id: array_ext.rb 5225 2015-01-12 16:40:00Z bmck $
# $(c): Copyright 2008-2015 by Capitalytics $

# Extend the core Ruby Array class with weighted-random-value functionality.

class Array

  def wrand(weights)
    total = weights.inject(0.0) { |t,w| t + w }
    point = Kernel.rand * total
    zip(weights).each do |n, w|
      return n if w >= point
      point -= w
    end
  end

  def nonempty?
    empty? ? false : true
  end

  def stringify
    to_json #.gsub(/\"/,"'")
  end

  # The code below implements binary search functionality on sorted arrays.
  # It was adapted from the code at http://0xcc.net/ruby-bsearch/index.html.en.

  def bsearch_lower_boundary(range = 0...length, &block)
    bsearch_boundary(true, range, &block)
  end

  def bsearch_upper_boundary(range = 0...length, &block)
    bsearch_boundary(false, range, &block)
  end

  # This method uses binary search to find the FIRST occurrence that satisfies
  # a condition given by a block, and returns its index. Returns nil if not found.
  def bsearch_first(range = 0...length, &block)
    boundary = bsearch_lower_boundary(range, &block)
    if boundary >= length || yield(self[boundary]) != 0
      return nil
    else
      return boundary
    end

  end

  alias bsearch bsearch_first

  # This method uses binary search to find the LAST occurrence that satisfies
  # a condition given by a block, and returns its index. Returns nil if not found.
  def bsearch_last(range = 0...length, &block)
    # `- 1' for canceling `lower + 1' in bsearch_upper_boundary.
    boundary = bsearch_upper_boundary(range, &block) - 1
    (boundary <= -1 || yield(self[boundary]) != 0) ? nil : boundary
  end

  # Return the search result as a Range object.
  def bsearch_range(range = 0...length, &block)
    lower = bsearch_lower_boundary(range, &block)
    upper = bsearch_upper_boundary(range, &block)
    lower...upper
  end

  # The code below provides convenience functions that use the binary search functionality above.

  def b_include?(obj) # assumes the array is sorted
    range = bsearch_range { |x| x <=> obj } # returned range excludes its last element
    (self[range.first] == obj or self[range.last - 1] == obj)
  end

  def b_first_index(obj)
    b_index(obj, true)
  end

  def b_last_index(obj)
    b_index(obj, false)
  end

  def combine(h = [])
    if length != h.length
      # max_index = [length,h.length].max
      (length - h.length).times { self << [] } if length > h.length
      (h.length - length).times { h << [] } if h.length > length
    end

    (0..(length - 1)).to_a.each { |i| self[i].combine(h[i]) }
    self
  end

  private

  def b_index(obj, flag) # assumes the array is sorted
    index = flag ? bsearch_first { |x| x <=> obj } : bsearch_last { |x| x <=> obj }
    self[index] == obj ? index : nil
  end

  def bsearch_boundary(flag, range = 0...length, &block)
    lower = (range.first - 1)
    upper = (range.exclude_end? ? range.last : (range.last + 1))
    while lower + 1 != upper
      mid = ((lower + upper) / 2).to_i
      y = yield(self[mid])
      if (y < 0) || (!flag && (y.zero?))
        lower = mid
      else
        upper = mid
      end

    end

    flag ? upper : (lower + 1) # outside of the matching range.
  end
end
