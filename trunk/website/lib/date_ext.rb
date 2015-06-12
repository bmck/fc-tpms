# $Id$
# $(c)$

# Extend the core Ruby Date class with new functionality.

class Date

  def start_of_month; self - (day - 1).day; end
  def start_of_quarter; start_of_month - ((month - 1) % 3).month; end
  def start_of_year; start_of_month - (month - 1).month; end

  def first_of_month; start_of_month; end
  def first_of_quarter; start_of_quarter; end
  def first_of_year; start_of_year; end

  def last_of_quarter; end_of_quarter; end
  def last_of_month; end_of_month; end
  def last_of_year; end_of_year; end

  def start_of_month?; (self == start_of_month); end
  def start_of_quarter?; (self == start_of_quarter); end
  def start_of_year?; (self == start_of_year); end

  def end_of_month?; (self == end_of_month); end
  def end_of_quarter?; (self == end_of_quarter); end
  def end_of_year?; (self == end_of_year); end

  def first_of_month?; start_of_month?; end
  def first_of_quarter?; start_of_quarter?; end
  def first_of_year?; start_of_year?; end

  def last_of_month?; end_of_month?; end
  def last_of_quarter?; end_of_quarter?; end
  def last_of_year?; end_of_year?; end

  def month_name; MONTHNAMES[month]; end
  def abbr_month_name; ABBR_MONTHNAMES[month]; end
  def month_number; year * 12 + month; end

  def quarter; (month + 2)/3; end
  def quarter_name; "#{quarter.ordinalize} Quarter"; end
  def abbr_quarter_name; "#{quarter}Q"; end

  def std_date_format; strftime(self.class.std_date_format); end
  def self.std_date_format; '%b %e, %Y'; end
  def std_daydate_format; strftime(self.class.std_daydate_format); end
  def self.std_daydate_format; '%a %b %d, %Y'; end
  def std_dbdate_format; strftime(self.class.std_dbdate_format); end
  def self.std_dbdate_format; '%Y-%m-%d'; end
  def std_shortdate_format; strftime(self.class.std_shortdate_format); end
  def self.std_shortdate_format; '%m/%d/%Y'; end
  def std_modate_format; strftime(self.class.std_modate_format); end
  def self.std_modate_format; '%B %Y'; end
  def bxs_shortdate_format; strftime(self.class.bxs_shortdate_format); end
  def self.bxs_shortdate_format; '%m/%d/%Y'; end

end
