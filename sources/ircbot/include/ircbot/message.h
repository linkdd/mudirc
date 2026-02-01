#pragma once

#include <preludec/defs.h>
#include <preludec/mem/alloc.h>
#include <preludec/mem/str.h>
#include <preludec/net/conn.h>


static constexpr usize irc_msg_max_params  = 15;
static constexpr usize irc_msg_buffer_size = 512;

typedef struct irc_msg_prefix irc_msg_prefix;
struct irc_msg_prefix {
  bool is_server;

  union {
    struct {
      str ident;
    } server;

    struct {
      str nick;
      str ident;
      str host;
    } user;
  };
};

typedef struct irc_msg irc_msg;
struct irc_msg {
  bool           has_prefix;
  irc_msg_prefix prefix;
  str            command;

  usize param_count;
  str   params[irc_msg_max_params];

  str trailing;
};


bool irc_msg_decode(irc_msg *self, str line);
str  irc_msg_encode(irc_msg *self, allocator a);

RESULT(UNIT, conn_error) irc_msg_send(irc_msg *self, conn_ref conn, allocator a);
