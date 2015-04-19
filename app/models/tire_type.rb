# $Id$
# $(c)$

class TireType < ActiveRecord::Base
  filterrific(
    default_filter_params: { sorted_by: 'name_asc' },
    available_filters: [
      :sorted_by,
      :search_query,
      :with_created_at_gte
    ]
  )
end
