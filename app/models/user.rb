# $Id$
# $(c)$

class User < ActiveRecord::Base
  has_paper_trail

  # Include default devise modules. Others available are:
  # :confirmable, :lockable, :timeoutable and :omniauthable
  devise :database_authenticatable, :registerable, \
    :recoverable, :rememberable, :trackable, :validatable, \
    :confirmable, :timeoutable

  after_update :send_password_change_email, if: :needs_password_change_email?

  scope :myself, -> user_id { where(id: user_id) }
  scope :same_company, -> user_company_id { where(company_id: user_company_id) }
  scope :all_users, -> {}

  scope :contains, -> x { joins('LEFT JOIN `companies` ON users.company_id = companies.id')
                          .where("locate(\"#{x}\", first_name) > 0 or " \
                                 "locate(\"#{x}\", last_name) > 0 or " \
                                 "locate(\"#{x}\", email) > 0 or " \
                                 "locate(\"#{x}\", name) > 0") }

  belongs_to :company

  cattr_reader :timeout_in do 2.hours end
  cattr_reader :valid_roles do %w(basic_user   company_admin   global_admin) end

  valid_roles.each do |x|
    define_method "#{x}?".to_sym do
      role == "#{x}"
    end
  end

  def first_last
    "#{first_name} #{last_name}"
  end

  def last_first
    "#{last_name}, #{first_name}"
  end

  def company
    return nil unless company_id
    Company.try(:find, company_id)
  end

  def company_name
    company.try(:name) || 'No Company Specified'
  end

  private

  # Change the logic here depending on how you use Devise.
  # For example, if you allow users to be created with just an email,
  # then this will always return true, so you'll need another thing to
  # check instead of `persisted?`
  #
  # The idea is that you want to differentiate between users who are signing
  # up for the first time (because `encrypted_password_changed?` will be true
  # for them), and those who are changing their password after having created
  # it for the first time.

  def needs_password_change_email?
    encrypted_password_changed? && persisted?
  end

  def send_password_change_email
    UserMailer.password_changed(id).deliver
  end
end
