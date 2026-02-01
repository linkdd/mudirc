#include <game/command.h>


cmd_result priv_command_unknown(bot *self, str from) {
  assert(self != NULL);

  allocator a = std_allocator();

  irc_msg resp     = {};
  resp.has_prefix  = false;
  resp.command     = str_literal("PRIVMSG");
  resp.param_count = 1;
  resp.params[0]   = from;
  resp.trailing    = str_literal("Unknown command.");

  auto res = irc_msg_send(&resp, self->conn, a);
  if (!res.is_ok) {
    return (cmd_result){
      .is_ok = false,
      .err   = strview_from_cstr(conn_strerror(res.err)),
    };
  }

  return (cmd_result){
    .is_ok = true,
  };
}
