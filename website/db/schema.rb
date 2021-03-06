# encoding: UTF-8
# This file is auto-generated from the current state of the database. Instead
# of editing this file, please use the migrations feature of Active Record to
# incrementally modify your database, and then regenerate this schema definition.
#
# Note that this schema.rb definition is the authoritative source for your
# database schema. If you need to create the application database on another
# system, you should be using db:schema:load, not running all the migrations
# from scratch. The latter is a flawed and unsustainable approach (the more migrations
# you'll amass, the slower it'll run and the greater likelihood for issues).
#
# It's strongly recommended that you check this file into your version control system.

ActiveRecord::Schema.define(version: 20151130024025) do

  create_table "authentication_tokens", force: :cascade do |t|
    t.string   "body",         limit: 255
    t.integer  "user_id",      limit: 4,   null: false
    t.datetime "last_used_at"
    t.string   "ip_address",   limit: 255
    t.string   "user_agent",   limit: 255
    t.datetime "created_at",               null: false
    t.datetime "updated_at",               null: false
  end

  add_index "authentication_tokens", ["user_id"], name: "index_authentication_tokens_on_user_id", using: :btree

  create_table "companies", force: :cascade do |t|
    t.string   "name",            limit: 64,                 null: false
    t.date     "start_service",                              null: false
    t.date     "end_service",                                null: false
    t.string   "contact_name",    limit: 64,                 null: false
    t.string   "contact_address", limit: 128,                null: false
    t.string   "contact_city",    limit: 32,                 null: false
    t.string   "contact_state",   limit: 2,                  null: false
    t.string   "contact_zip",     limit: 10,                 null: false
    t.string   "contact_phone",   limit: 15,                 null: false
    t.string   "contact_email",   limit: 64,                 null: false
    t.string   "domain_name",     limit: 128,                null: false
    t.datetime "created_at",                                 null: false
    t.datetime "updated_at",                                 null: false
    t.boolean  "active",                      default: true, null: false
    t.integer  "time_threshold",  limit: 4
  end

  create_table "receivers", force: :cascade do |t|
    t.string   "serial",        limit: 255,                null: false
    t.string   "receiver_type", limit: 255
    t.datetime "created_at",                               null: false
    t.datetime "updated_at",                               null: false
    t.boolean  "active",                    default: true, null: false
  end

  create_table "sensors", force: :cascade do |t|
    t.string   "serial",      limit: 255,                null: false
    t.string   "sensor_type", limit: 255
    t.datetime "created_at",                             null: false
    t.datetime "updated_at",                             null: false
    t.boolean  "active",                  default: true, null: false
  end

  create_table "sessions", force: :cascade do |t|
    t.string   "session_id", limit: 255,   null: false
    t.text     "data",       limit: 65535
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  add_index "sessions", ["session_id"], name: "index_sessions_on_session_id", unique: true, using: :btree
  add_index "sessions", ["updated_at"], name: "index_sessions_on_updated_at", using: :btree

  create_table "tire_locations", force: :cascade do |t|
    t.string   "type",            limit: 255,                null: false
    t.integer  "company_id",      limit: 4,                  null: false
    t.string   "truck_serial",    limit: 255
    t.string   "trailer_serial",  limit: 255
    t.string   "storage_name",    limit: 255
    t.string   "storage_address", limit: 255
    t.string   "storage_city",    limit: 255
    t.string   "storage_state",   limit: 255
    t.string   "location_name",   limit: 255,                null: false
    t.datetime "created_at",                                 null: false
    t.datetime "updated_at",                                 null: false
    t.boolean  "active",                      default: true, null: false
  end

  create_table "tire_samples", force: :cascade do |t|
    t.integer  "sensor_id",   limit: 4,                                         null: false
    t.integer  "receiver_id", limit: 4,                                         null: false
    t.decimal  "psi",                   precision: 10, scale: 6,                null: false
    t.datetime "sample_time"
    t.datetime "created_at",                                                    null: false
    t.datetime "updated_at",                                                    null: false
    t.boolean  "active",                                         default: true, null: false
    t.integer  "tempc",       limit: 4
  end

  create_table "tire_types", force: :cascade do |t|
    t.string   "name",             limit: 255,                null: false
    t.datetime "created_at",                                  null: false
    t.datetime "updated_at",                                  null: false
    t.boolean  "active",                       default: true, null: false
    t.integer  "time_threshold",   limit: 4
    t.float    "hi_psi_threshold", limit: 24
    t.float    "lo_psi_threshold", limit: 24
    t.float    "nominal_psi",      limit: 24,  default: 0.0
  end

  create_table "tires", force: :cascade do |t|
    t.integer  "sensor_id",         limit: 4,                  null: false
    t.integer  "tire_type_id",      limit: 4,                  null: false
    t.integer  "using_company_id",  limit: 4,                  null: false
    t.integer  "owning_company_id", limit: 4,                  null: false
    t.integer  "tire_location_id",  limit: 4,                  null: false
    t.string   "location_notation", limit: 255
    t.string   "serial",            limit: 255
    t.datetime "created_at",                                   null: false
    t.datetime "updated_at",                                   null: false
    t.boolean  "active",                        default: true, null: false
    t.integer  "time_threshold",    limit: 4
    t.float    "hi_psi_threshold",  limit: 24
    t.float    "lo_psi_threshold",  limit: 24
  end

  create_table "users", force: :cascade do |t|
    t.string   "first_name",             limit: 255,                        null: false
    t.string   "last_name",              limit: 255,                        null: false
    t.string   "role",                   limit: 255, default: "basic_user", null: false
    t.integer  "company_id",             limit: 4
    t.datetime "created_at",                                                null: false
    t.datetime "updated_at",                                                null: false
    t.string   "email",                  limit: 255, default: "",           null: false
    t.string   "encrypted_password",     limit: 255, default: "",           null: false
    t.string   "reset_password_token",   limit: 255
    t.datetime "reset_password_sent_at"
    t.datetime "remember_created_at"
    t.integer  "sign_in_count",          limit: 4,   default: 0,            null: false
    t.datetime "current_sign_in_at"
    t.datetime "last_sign_in_at"
    t.string   "current_sign_in_ip",     limit: 255
    t.string   "last_sign_in_ip",        limit: 255
    t.string   "confirmation_token",     limit: 255
    t.datetime "confirmed_at"
    t.datetime "confirmation_sent_at"
    t.string   "unconfirmed_email",      limit: 255
    t.integer  "failed_attempts",        limit: 4,   default: 0,            null: false
    t.string   "unlock_token",           limit: 255
    t.datetime "locked_at"
    t.boolean  "active",                             default: true,         null: false
    t.integer  "time_threshold",         limit: 4
  end

  add_index "users", ["confirmation_token"], name: "index_users_on_confirmation_token", unique: true, using: :btree
  add_index "users", ["email"], name: "index_users_on_email", unique: true, using: :btree
  add_index "users", ["reset_password_token"], name: "index_users_on_reset_password_token", unique: true, using: :btree
  add_index "users", ["unlock_token"], name: "index_users_on_unlock_token", unique: true, using: :btree

  create_table "versions", force: :cascade do |t|
    t.string   "item_type",  limit: 255,   null: false
    t.integer  "item_id",    limit: 4,     null: false
    t.string   "event",      limit: 255,   null: false
    t.string   "whodunnit",  limit: 255
    t.text     "object",     limit: 65535
    t.datetime "created_at"
  end

  add_index "versions", ["item_type", "item_id"], name: "index_versions_on_item_type_and_item_id", using: :btree

end
