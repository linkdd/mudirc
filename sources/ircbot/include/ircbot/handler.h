#pragma once

#include <ircbot/message.h>


typedef RESULT(UNIT, str) irc_error;

typedef struct irc_handler irc_handler;
struct irc_handler {
  irc_error (*welcome)  (void *udata, irc_msg *msg);
  irc_error (*nickinuse)(void *udata, irc_msg *msg);

  irc_error (*ping)   (void *udata, irc_msg *msg);
  irc_error (*privmsg)(void *udata, irc_msg *msg);
  irc_error (*notice) (void *udata, irc_msg *msg);
  irc_error (*join)   (void *udata, irc_msg *msg);
  irc_error (*part)   (void *udata, irc_msg *msg);
  irc_error (*quit)   (void *udata, irc_msg *msg);
  irc_error (*nick)   (void *udata, irc_msg *msg);
  irc_error (*mode)   (void *udata, irc_msg *msg);

  irc_error (*fallback)(void *udata, irc_msg *msg);

  void *udata;
};


irc_error irc_handler_call(irc_handler self, irc_msg *msg);
