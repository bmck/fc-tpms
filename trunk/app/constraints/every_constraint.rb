# $Id$
# $(c)$

class EveryConstraint
  def initialize(constraint_objs)
    @constraints = [constraint_objs].flatten
  end

  def matches?(r)
    @constraints.each { |c| return false unless c.matches?(r) }
    true
  end
end
