# $Id$
# $(c)$

User.create!([
               {first_name: "Bill", last_name: "McKinnon", role: "global_admin", company_id: nil, email: "bmck@bmck.org", encrypted_password: "$2a$10$H4dlsXqIdBDIT0Y45qFs/O9TWfpMDl2NCnMGwQAXOcmV3QnrMNbkm", reset_password_token: nil, reset_password_sent_at: nil, remember_created_at: nil, sign_in_count: 1, current_sign_in_at: "2015-04-22 16:24:17", last_sign_in_at: "2015-04-22 16:24:17", current_sign_in_ip: "127.0.0.1", last_sign_in_ip: "127.0.0.1", confirmation_token: nil, confirmed_at: "2015-04-22 16:24:01", confirmation_sent_at: "2015-04-22 16:07:28", unconfirmed_email: nil, failed_attempts: 0, unlock_token: nil, locked_at: nil},
               {first_name: "Martin", last_name: "McKinnon", role: "basic_user", company_id: nil, email: "bill@bmck.org", encrypted_password: "$2a$10$bF8E6W0WIfuA41.F1AIsXOKCOymt8at3lXWCZRMsmYrcgMK8ebkf6", reset_password_token: nil, reset_password_sent_at: nil, remember_created_at: nil, sign_in_count: 0, current_sign_in_at: nil, last_sign_in_at: nil, current_sign_in_ip: nil, last_sign_in_ip: nil, confirmation_token: "f7a649e3abd7066a5c6173b6efdaead7818c4f3ccf0b7671368fdc7ebda990e4", confirmed_at: nil, confirmation_sent_at: "2015-04-22 17:44:54", unconfirmed_email: nil, failed_attempts: 0, unlock_token: nil, locked_at: nil}
])
Company.create!([
                  {name: "CRST", start_service: "2015-04-22", end_service: "2018-04-22", contact_name: "James Hanie", contact_address: "435 20th St NW", contact_city: "Cedar Rapids", contact_state: "IA", contact_zip: "76532", contact_phone: "201-444-9090", contact_email: "jhanie@crst.com", domain_name: "crst.com"},
                  {name: "UPS", start_service: "2015-04-22", end_service: "2016-04-22", contact_name: "Bob Hutchins", contact_address: "677 North Ave.", contact_city: "Atlanta", contact_state: "GA", contact_zip: "30332", contact_phone: "404-565-9022", contact_email: "bhutchins@ups.com", domain_name: "ups.com"}
])
Tire.create!([
               {sensor_id: 333, tire_type_id: 2, company_id: nil}
])
TireType.create!([
                   {name: "Yoyodyne BigAss Tire"},
                   {name: "Another yoyodyne BAT2"},
                   {name: "Still another yoyodyne BAT"},
                   {name: "BAT"},
                   {name: "BAT2"},
                   {name: "still testing tires really"},
                   {name: "yawn more test tire types"},
                   {name: "why more tires"}
])
