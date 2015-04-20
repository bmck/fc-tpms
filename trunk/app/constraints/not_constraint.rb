# $Id: not_constraint.rb 5180 2015-01-03 22:03:32Z bmck $
# $(c): Copyright 2008-2015 by Capitalytics $

class NotConstraint
  def initialize(constraint_objs)
    @constraints = [constraint_objs].flatten
  end

  def matches?(r)
    @constraints.each { |c| return false if c.matches?(r) }
    true
  end
end
