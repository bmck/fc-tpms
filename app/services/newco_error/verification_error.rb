# $Id$
# $(c)$

module NewcoError
  class VerificationError < ::StandardError

    def initialize(request)
      @url, @whence = request.original_url, DateTime.now
      bad_fn = Rails.root.join('etc', Settings.bad_samples_file)
      CSV.open(bad_fn, 'a') do |bad_file|
        bad_file << [DateTime.now, @url]
      end

      @msg = "#{@whence}: An unverified tire sample was presented (#{@url})"
    end
  end
end
