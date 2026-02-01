#pragma once

#include <preludec/defs.h>
#include <preludec/mem/str.h>
#include <preludec/net/conn.h>

#include <ircbot/handler.h>


static constexpr usize irc_client_buffer_size = 4096;


typedef struct irc_client irc_client;
struct irc_client {
  conn_ref    conn;
  irc_handler handler;

  str   buffer;
  char _bufmem[irc_client_buffer_size];
};


void irc_client_init(irc_client *self, conn_ref conn, irc_handler handler);

RESULT(bool, str) irc_client_consume(irc_client *self);
