

module TracerBullets

  module Methods
    def tracer_bullet
      if Rails.env.development? || Rails.env.test?
        _tracer_bullets_log( "Elapsed: #{((Time.now - @tracer_bullet_start_time)*1000).to_i}ms #{caller(0)[1]}" )
        @tracer_bullet_start_time = Time.now
      end
    end
  end

end
