#pragma once

#include <preludec/net/conn.h>
#include <ircbot/handler.h>

#include <game/database/conn.h>


typedef struct bot bot;
struct bot {
  dbconn   *db;
  conn_ref  conn;

  str nick;
};


RESULT(UNIT, str) bot_init  (bot *self, dbconn *db, conn_ref c, str nick);
void              bot_deinit(bot *self);

irc_handler bot_handler(bot *self);
