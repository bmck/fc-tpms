# $Id$
# $(c)$

class AnyConstraint
  def initialize(constraint_objs)
    @constraints = [constraint_objs].flatten
  end

  def matches?(r)
    @constraints.each { |c| return true if c.matches?(r) }
    false
  end
end
