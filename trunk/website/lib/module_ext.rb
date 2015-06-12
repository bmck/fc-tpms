# $Id$
# $(c)$

class Module
  def namespace
    name.split('::')
  end
end
