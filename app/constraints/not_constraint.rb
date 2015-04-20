# $Id$
# $(c)$

class NotConstraint
  def initialize(constraint_objs)
    @constraints = [constraint_objs].flatten
  end

  def matches?(r)
    @constraints.each { |c| return false if c.matches?(r) }
    true
  end
end
