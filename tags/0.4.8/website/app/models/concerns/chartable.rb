# $Id$
# $(c)$

module Chartable
  extend ActiveSupport::Concern
  include ActionView::Helpers::AssetUrlHelper

  def line_chart
    serial_chart(line_chart_params)
  end

  protected

  def chart_bullet(num)
    chart_bullets(num).last
  end

  def chart_bullets(num)
    return @chart_bullets[0 ... num] if @chart_bullets && num < @chart_bullets.length

    uniq_bullets = %w(round square triangleUp triangleDown bubble )
    num_cycles = (num.to_f / uniq_bullets.length.to_f).ceil

    @chart_bullets = (uniq_bullets * num_cycles)[0 ... num]
  end

  def chart_color(num)
    chart_colors(num).last
  end

  def chart_colors(num)
    return @chart_colors[0 ... num] if @chart_colors && num < @chart_colors.length

    cols = []

    color_maker = ColorGenerator.new(saturation: 0.9, hue: 0.1, value: 0.95, seed: 4)
    num.times { cols << color_maker.create_hex }
    @chart_colors = cols
  end
end
