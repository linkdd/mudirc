#include <game/command.h>


cmd_result game_command_unknown(bot *self, str channel, str from) {
  assert(self != NULL);

  allocator a = std_allocator();

  irc_msg m_resp     = {};
  m_resp.has_prefix  = false;
  m_resp.command     = str_literal("PRIVMSG");
  m_resp.param_count = 1;
  m_resp.params[0]   = channel;
  m_resp.trailing    = str_join(a, from, str_literal(": Unknown command."));

  auto res = irc_msg_send(&m_resp, self->conn, a);
  str_free(a, &m_resp.trailing);

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
