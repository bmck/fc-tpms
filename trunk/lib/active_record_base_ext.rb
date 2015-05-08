# $Id$
# $(c)$

module ActiveRecord

  # Following is from https://gist.github.com/jrochkind/59b6330e3f52710cc49e and https://bibwild.wordpress.com/2014/07/17/activerecord-concurrency-in-rails4-avoid-leaked-connections/

  # We're refusing to give a connection when asked for. Same outcome
  # as if the pool timed out on checkout, so let's subclass the exception
  # used for that.
  class ImplicitConnectionForbiddenError < ::ActiveRecord::ConnectionTimeoutError ; end

  class Base
    class << self

      ######################
      #
      #  Monkey patch to ActiveRecord to prevent 'implicit' checkouts. Currently tested with Rails 4.0.8, prob
      #  should work fine in Rails 4.1 too.
      #
      #  If you create a thread yourself, if it uses ActiveRecord objects without
      #  explicitly checking out a connection, one will still be checked out implicitly.
      #  If it is never checked back in with `ActiveRecord::Base.clear_active_connections!`,
      #  then it will be leaked.
      #
      #  For some uses, we want to avoid being able to do that kind of implicit checkout,
      #  force all ActiveRecord use to be via an explicit checkout using with_connection
      #  or checkout.
      #
      #  With this monkey patch, a thread can call:
      #
      #      ActiveRecord::Base.forbid_implicit_checkout_for_thread!
      #
      #  And subsequently, if that thread accidentally tries to do an implicit
      #  checkout, an exception will be raised.
      #
      #  The exception raised is defined here as ActiveRecord::ImplicitConnectionForbiddenError < ActiveRecord::ConnectionTimeoutError
      #
      ##########################

      def forbid_implicit_checkout_for_thread!
        Thread.current[:active_record_forbid_implicit_connections] = true
      end

      def connection_with_forbid_implicit(*args, &block)
        if ( Thread.current[:active_record_forbid_implicit_connections] &&
             ! connection_handler.retrieve_connection_pool(self).active_connection?)

          msg = "Implicit ActiveRecord checkout attempted when Thread :force_explicit_connections set! at \n" +
            caller.join("\n") + "\n\n\n"

          # I want to make SURE I see this error in test output, even though
          # in some cases my code is swallowing the exception.
          if Rails.env.test?
            $stderr.puts msg
          end

          raise ImplicitConnectionForbiddenError.new(msg)
        end
        connection_without_forbid_implicit(*args, &block)
      end

      alias_method_chain :connection, :forbid_implicit

      # End functionality from https://gist.github.com/jrochkind/59b6330e3f52710cc49e

      # Extend the core Ruby ActiveRecord::Base class with new record validation functionality.

      def escape_sql(sql)
        send(:sanitize_sql, sql)
      end

      def execute_sql(sql)
        connection.execute(escape_sql(sql))
      end

      def ids(tbl_name = quoted_table_name)
        execute_sql("select distinct id from #{tbl_name}").map { |x| x[0].to_i }.sort
      end

      def all_tables
        connection.tables - ['schema_migrations']
      end

      # Much of the following is taken from https://github.com/jsuchal/activerecord-fast-import
      # Deletes all rows in table very fast, but without calling +destroy+ method
      # nor any hooks.
      def truncate_table(tbl_name = quoted_table_name)
        execute_sql("TRUNCATE TABLE #{tbl_name}")
      end

      # Disables key updates for model table
      def disable_keys(tbl_name = quoted_table_name)
        execute_sql("ALTER TABLE #{tbl_name} DISABLE KEYS")
      end

      # Enables key updates for model table
      def enable_keys(tbl_name = quoted_table_name)
        execute_sql("ALTER TABLE #{tbl_name} ENABLE KEYS")
      end

      # Disables keys, yields block, enables keys.
      def with_keys_disabled
        disable_keys
        yield
        enable_keys
      end

      # Loads data from file(s) using MySQL native LOAD DATA INFILE query, disabling
      # key updates for even faster import speed
      #
      # ==== Parameters
      # * +files+ file(s) to import
      # * +options+ (see <tt>load_data_infile</tt>)
      def fast_import(files, options = {})
        files = Array(files) # [files] unless files.is_a? Array
        with_keys_disabled do
          load_data_infile_multiple(files, options)
        end
      end

      # Loads data from file using MySQL native LOAD DATA INFILE query
      #
      # ==== Parameters
      # * +file+ the file to import
      # * +options+
      def load_data_infile(files, options = {})
        Array(files).each do |file|
          tbl_name = options.fetch(:tbl_name) { quoted_table_name }

          sql = "LOAD DATA INFILE '#{file}' "
          sql << "#{options[:insert_method]} " if options[:insert_method]
          sql << "INTO TABLE #{tbl_name} "
          sql << "CHARACTER SET #{options[:charset_name]} " if options[:charset_name]

          fields = ''
          fields << "TERMINATED BY #{structure_string options[:fields_terminated_by]} " if options[:fields_terminated_by]
          fields << "OPTIONALLY ENCLOSED BY '#{options[:fields_optionally_enclosed_by]}' " if options[:fields_optionally_enclosed_by]
          fields << "ESCAPED BY '#{options[:fields_escaped_by]}' " if options[:fields_escaped_by]

          sql << "FIELDS #{fields} " unless fields.empty?
          sql << "LINES TERMINATED BY #{structure_string options[:lines_terminated_by]} " if options[:lines_terminated_by]
          sql << "IGNORE #{options[:ignore_lines]} LINES " if options[:ignore_lines]
          sql << '(' + options[:columns].join(', ') + ') ' if options[:columns]
          if options[:mapping]
            mappings = []
            options[:mapping].each_pair do |column, mapping|
              mappings << "#{column} = #{mapping}"
            end
            sql << "SET #{mappings.join(', ')} " unless mappings.empty?
          end
          sql << ';'
          execute_sql(sql)
        end
      end

      alias_method :load_data_infiles, :load_data_infile
      alias_method :load_data_infile_multiple, :load_data_infile

      private

      def structure_string(spec)
        if spec.is_a? Fixnum
          "0x%x" % spec
        else
          "'#{spec}'"
        end
      end
    end

    private

    def validate_enum_attr(attr_name, opts)
      attrib = send(attr_name)
      validate_required_attr(attr_name, opts, attrib, errors)
      errors.add(attr_name, 'must be a symbol') if attrib.try(:is_a?, Symbol) == false
      legal_values = column_for_attribute(attr_name).limit
      errors.add(attr_name, 'is !a legal value') if attrib.try(:is_a?, Symbol) == true && !legal_values.include?(attrib)
    end

    def validate_string_attr(attr_name, opts)
      attrib = send(attr_name)
      errors.add(attr_name, 'is required') if attrib.blank? && !opts[:null]
      errors.add(attr_name, 'must be a text string') if attrib.try(:is_a?, String) == false
      max_len = column_for_attribute(attr_name).limit
      errors.add(attr_name, "cannot exceed #{max_len} character(s)") if attrib.try(:is_a?, String) == true && attrib.length > max_len
    end

    def validate_integer_attr(attr_name, opts)
      attrib = send(attr_name)
      validate_required_attr(attr_name, opts, attrib, errors)
      errors.add(attr_name, 'must be an integer') if attrib && ((!attrib.is_a?(Fixnum) && !attrib.is_a?(String)) ||
                                                                attrib.to_s != attrib.to_i.to_s)
      send((attr_name + '='), attrib.to_i) unless errors[attr_name] || (attrib.nil? && opts[:null])
    end

    def validate_whole_number_attr(attr_name, opts)
      validate_bounded_fixnum_attr(attr_name, opts, 'must be a non-negative number', 0)
    end

    def validate_natural_attr(attr_name, opts)
      validate_bounded_fixnum_attr(attr_name, opts, 'must be greater than zero', 1)
    end

    def validate_bounded_fixnum_attr(attr_name, opts, error_text, min_val)
      validate_integer_attr(attr_name, opts)
      attrib = send(attr_name)
      errors.add(attr_name, error_text) if attrib && !opts[:null] && attrib.is_a?(Fixnum) && attrib < min_val
    end

    def validate_float_attr(attr_name, opts)
      validate_floatpoint_attr(attr_name, opts, 'must be a floating-point number', Float)
    end

    def validate_bigdecimal_attr(attr_name, opts)
      validate_floatpoint_attr(attr_name, opts, 'must be a big-decimal number', BigDecimal)
    end

    def validate_floatpoint_attr(attr_name, opts, error_text, klass)
      attrib = send(attr_name)
      validate_required_attr(attr_name, opts, attrib, errors)
      errors.add(attr_name, error_text) if attrib && !attrib.is_a?(klass) && !attrib.is_a?(String)
      send((attr_name + '='), attrib.to_f) unless errors[attr_name]
    end

    def validate_boolean_attr(attr_name, opts)
      attrib = send(attr_name)
      validate_required_attr(attr_name, opts, attrib, errors)
      errors.add(attr_name, 'must contain a Boolean value') if attrib && attrib.class != TrueClass && attrib.class != FalseClass
    end

    def validate_date_attr(attr_name, opts)
      attrib = send(attr_name)
      validate_required_attr(attr_name, opts, attrib, errors)
      errors.add(attr_name, 'must be a valid date') if attrib && ((!attrib.is_a?(Date) && !attrib.is_a?(String)) ||
                                                                  (attrib.is_a?(String) && !attrib.to_date_or_nil))
      send((attr_name + '='), attrib.to_date) unless attrib.nil? || errors[attr_name]
    end

    def validate_datetime_attr(attr_name, opts)
      attrib = send(attr_name)
      validate_required_attr(attr_name, opts, attrib, errors)
      errors.add(attr_name, 'must be a valid datetime') if attrib && ((!attrib.is_a?(DateTime) && !attrib.is_a?(Time) &&
                                                                      !attrib.is_a?(String)) || (attrib.is_a?(String) &&
                                                                                                 !attrib.to_datetime_or_nil))
      send((attr_name + '='), attrib.to_datetime) unless attrib.nil? || errors[attr_name]
    end

    def validate_foreign_key_attr(attr_name, class_name, opts)
      validate_whole_number_attr(attr_name, opts)
      attrib = send(attr_name)
      begin
        class_name.find(attrib) if attrib
      rescue ActiveRecord::RecordNotFound
        errors.add(attr_name, "must refer to a valid #{class_name} ID")
      end
    end

    def validate_required_attr(attr_name, opts, attrib, errors)
      errors.add(attr_name, 'is required') if attrib.nil? &&  !opts[:null]
    end
  end
end
