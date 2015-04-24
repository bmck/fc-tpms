# $Id$
# $(c)$

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
