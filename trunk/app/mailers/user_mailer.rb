# $Id$
# $(c)$

class UserMailer < ActionMailer::Base
  default from: "some-email@your-domain.ext"

  def password_changed(id)
    @user = User.find(id)
    mail to: @user.email, subject: "Your password has changed"
  end
end
