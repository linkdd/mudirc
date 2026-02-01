#include <game/command.h>


cmd_error priv_command_unknown(bot *self, str from) {
  assert(self != NULL);

  allocator a = std_allocator();

  irc_msg m_resp     = {};
  m_resp.has_prefix  = false;
  m_resp.command     = str_literal("PRIVMSG");
  m_resp.param_count = 1;
  m_resp.params[0]   = from;
  m_resp.trailing    = str_literal("Unknown command.");

  str s_resp = irc_msg_encode(&m_resp, a);
  RESULT(UNIT, conn_error) res = conn_write(self->conn, s_resp);
  str_free(a, &s_resp);

  if (!res.is_ok) {
    return (cmd_error){
      .is_ok = false,
      .err   = strview_from_cstr(conn_strerror(res.err)),
    };
  }

  return (cmd_error){
    .is_ok = true,
  };
}
