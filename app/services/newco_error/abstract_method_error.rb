# $Id$
# $(c)$

module NewcoError
  class AbstractMethodError < ::StandardError
    def initialize
      @msg = "An unspecified abstract method execution was attempted.\nCall stack: #{caller[0..8].join("\n")}"
    end
  end
end
