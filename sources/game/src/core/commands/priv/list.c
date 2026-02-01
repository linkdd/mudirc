#include <game/core/command.h>


cmd_result priv_command_list(bot *self, str from, priv_command *cmd) {
  assert(self != NULL);
  assert(cmd  != NULL);

  irc_msg resp     = {};
  resp.has_prefix  = false;
  resp.command     = str_literal("PRIVMSG");
  resp.param_count = 1;
  resp.params[0]   = from;
  resp.trailing    = str_literal("Not yet implemented.");

  auto res = irc_msg_send(&resp, self->conn, std_allocator());
  if (!res.is_ok) {
    return (cmd_result) ERR(strview_from_cstr(conn_strerror(res.err)));
  }

  return (cmd_result) OK({});
}
