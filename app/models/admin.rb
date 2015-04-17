# $Id$
# $(c)$

class Admin < ActiveRecord::Base
  # Include default devise modules. Others available are:
  # :confirmable, :lockable, :timeoutable and :omniauthable
  devise :database_authenticatable, :timeoutable
  # :recoverable, :rememberable, :trackable, :validatable
end
