#pragma once

#include <ircbot/message.h>


typedef RESULT(UNIT, str) irc_result;

typedef struct irc_handler irc_handler;
struct irc_handler {
  irc_result (*welcome)  (void *udata, irc_msg *msg);
  irc_result (*nickinuse)(void *udata, irc_msg *msg);

  irc_result (*ping)   (void *udata, irc_msg *msg);
  irc_result (*privmsg)(void *udata, irc_msg *msg);
  irc_result (*notice) (void *udata, irc_msg *msg);
  irc_result (*join)   (void *udata, irc_msg *msg);
  irc_result (*part)   (void *udata, irc_msg *msg);
  irc_result (*quit)   (void *udata, irc_msg *msg);
  irc_result (*nick)   (void *udata, irc_msg *msg);
  irc_result (*mode)   (void *udata, irc_msg *msg);

  irc_result (*fallback)(void *udata, irc_msg *msg);

  void *udata;
};


irc_result irc_handler_call(irc_handler self, irc_msg *msg);
