# $Id: active-record-query-trace.rb 5671 2015-04-06 15:06:43Z bmck $
# $(c): Copyright 2008-2015 by Capitalytics $

if defined? ActiveRecordQueryTrace
  ActiveRecordQueryTrace.enabled = false
  if Rails.env.development?
    ActiveRecordQueryTrace.enabled = false
    ActiveRecordQueryTrace.level = :app # default
    ActiveRecordQueryTrace.lines = 10
  end
  if Rails.env.test?
    ActiveRecordQueryTrace.enabled = false
    ActiveRecordQueryTrace.level = :app # default
    ActiveRecordQueryTrace.lines = 5
  end
end
