#pragma once

#include <ircbot/message.h>


typedef struct irc_handler irc_handler;
struct irc_handler {
  RESULT(UNIT, str) (*welcome)  (void *udata, irc_msg *msg);
  RESULT(UNIT, str) (*nickinuse)(void *udata, irc_msg *msg);

  RESULT(UNIT, str) (*ping)   (void *udata, irc_msg *msg);
  RESULT(UNIT, str) (*privmsg)(void *udata, irc_msg *msg);
  RESULT(UNIT, str) (*notice) (void *udata, irc_msg *msg);
  RESULT(UNIT, str) (*join)   (void *udata, irc_msg *msg);
  RESULT(UNIT, str) (*part)   (void *udata, irc_msg *msg);
  RESULT(UNIT, str) (*quit)   (void *udata, irc_msg *msg);
  RESULT(UNIT, str) (*nick)   (void *udata, irc_msg *msg);
  RESULT(UNIT, str) (*mode)   (void *udata, irc_msg *msg);

  RESULT(UNIT, str) (*fallback)(void *udata, irc_msg *msg);

  void *udata;
};


RESULT(UNIT, str) irc_handler_call(irc_handler self, irc_msg *msg);
