# $Id$
# $(c)$

class XhrConstraint
  def matches?(r)
    r.xhr?
  end
end
