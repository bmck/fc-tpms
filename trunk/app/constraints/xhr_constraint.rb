# $Id: xhr_constraint.rb 5180 2015-01-03 22:03:32Z bmck $
# $(c): Copyright 2008-2015 by Capitalytics $

class XhrConstraint
  def matches?(r)
    r.xhr?
  end
end
