# $Id: hash_ext.rb 5180 2015-01-03 22:03:32Z bmck $
# $(c): Copyright 2008-2015 by Capitalytics $

# Extend the core Ruby Hash class with Capitalytics-specific functionality.
class Hash

  def to_json_mod(options = nil) #:nodoc:
    hash = as_json(options)

    result = '{'
    result << hash.map do |key, value|
      "#{ActiveSupport::JSON.encode(key.to_s)[1..-2].html_safe}: #{ActiveSupport::JSON.encode(value, options).html_safe}"
    end * ', '
    result << '}'
  end

  def to_cgi_string
    # self.keys.inject('') do |query_string, key|
    #   query_string << '&' unless key == self.keys.first
    #   query_string << "#{URI.encode(key.to_s)}=#{URI.encode(self[key].to_s)}"
    # end
    to_params
  end

  # Convert to URL params per http://stackoverflow.com/questions/798710/how-to-turn-a-ruby-hash-into-http-params
  def to_params(no_include = [])
    uri = Addressable::URI.new
    uri.query_values = except(*(ApplicationController.screened_param_keys)).except(*no_include)
    return uri.query
  end

  def to_sorted_params(no_include = [])
    to_params(no_include).split('&').sort.join('&')
  end

  def from_array(array = [])
    h = {}
    array.size.times do |t|
      h[t] = array[t]
    end
    h
  end

end
