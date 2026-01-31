#include <game/event-loop.h>
#include <game/lifecycle.h>

#include <ircbot/message.h>


RESULT(UNIT, str) event_loop(bot *b) {
  assert(b != NULL);

  char bufmem[irc_msg_buffer_size] = {};
  str buffer = {
    .data     = bufmem,
    .length   = 0,
    .capacity = irc_msg_buffer_size,
    .owned    = false,
  };

  while (lc_running()) {
    memset(buffer.data, 0, buffer.capacity);
    RESULT(UNIT, conn_error) c_res = conn_read(b->conn, &buffer);
    if (!c_res.is_ok) {
      if (c_res.err != CONN_ERR_CLOSED) {
        return (RESULT(UNIT, str)){
          .is_ok = false,
          .err   = strview_from_cstr(conn_strerror(c_res.err)),
        };
      }
      else {
        break;
      }
    }

    irc_msg msg = {};
    if (irc_msg_decode(&msg, buffer)) {
      RESULT(UNIT, str) res = irc_handler_call(bot_handler(b), &msg);
      if (!res.is_ok) {
        return res;
      }
    }
  }

  return (RESULT(UNIT, str)){
    .is_ok = true,
  };
}
