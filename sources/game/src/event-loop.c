#include <game/event-loop.h>
#include <game/lifecycle.h>

#include <ircbot/message.h>


RESULT(UNIT, str) event_loop(bot *b) {
  assert(b != NULL);

  constexpr usize bufsz = 4096;
  char bufmem[bufsz] = {};
  str buffer = {
    .data     = bufmem,
    .length   = 0,
    .capacity = bufsz,
    .owned    = false,
  };

  while (lc_running()) {
    str view = str_slice(buffer, buffer.length, buffer.capacity);
    RESULT(UNIT, conn_error) c_res = conn_read(b->conn, &view);
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
    buffer.length += view.length;

    do {
      usize end_of_message = 0;
      for (usize i = 0; i + 1 < buffer.length; ++i) {
        if (buffer.data[i] == '\r' && buffer.data[i + 1] == '\n') {
          end_of_message = i + 2;
          break;
        }
      }

      if (end_of_message == 0) {
        break;
      }

      str     line = str_slice(buffer, 0, end_of_message);
      irc_msg msg  = {};
      if (irc_msg_decode(&msg, line)) {
        RESULT(UNIT, str) res = irc_handler_call(bot_handler(b), &msg);
        if (!res.is_ok) {
          return res;
        }
      }

      memmove(
        buffer.data,
        buffer.data + end_of_message,
        buffer.length - end_of_message
      );
      buffer.length -= end_of_message;
    } while (true);
  }

  return (RESULT(UNIT, str)){
    .is_ok = true,
  };
}
