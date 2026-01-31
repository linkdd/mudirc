#pragma once

#include <preludec/net/conn.h>
#include <ircbot/handler.h>

#include <sqlite3.h>


typedef struct bot bot;
struct bot {
  sqlite3  *db;
  conn_ref  conn;

  str nick;
};


RESULT(UNIT, str) bot_init  (bot *self, const char *dbpath, conn_ref c, str nick);
void              bot_deinit(bot *self);


irc_handler bot_handler(bot *self);
