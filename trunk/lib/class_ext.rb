# $Id$
# $(c)$

class Class
  def self.exists?(klass_name)
    begin
      mod = klass_name.namespace[0..-2].join('::').constantize
      base_klass_name = klass_name.namespace[-1]
      return (mod.constants.include?(base_klass_name) && ("#{klass_name}".constantize.is_a?(Class)))
    rescue NameError
      return false
    end
  end
end
