#include <game/command.h>


cmd_result game_command_query(bot *self, str channel, str from, game_command *cmd) {
  assert(self != NULL);
  assert(cmd  != NULL);

  allocator a = std_allocator();

  irc_msg resp     = {};
  resp.has_prefix  = false;
  resp.command     = str_literal("PRIVMSG");
  resp.param_count = 1;
  resp.params[0]   = channel;
  resp.trailing    = str_join(a, from, str_literal(": Not implemented yet."));

  RESULT(UNIT, conn_error) res = irc_msg_send(&resp, self->conn, a);
  str_free(a, &resp.trailing);

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