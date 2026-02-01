#include <game/event-loop.h>
#include <game/lifecycle.h>

#include <ircbot/client.h>


RESULT(UNIT, str) event_loop(bot *b) {
  assert(b != NULL);

  irc_client c = {};
  irc_client_init(&c, b->conn, bot_handler(b));

  while (lc_running()) {
    auto res = irc_client_consume(&c);
    if (!res.is_ok) {
      return (RESULT(UNIT, str)) ERR(res.err);
    }
    else {
      bool should_continue = res.ok;
      if (!should_continue) break;
    }
  }

  return (RESULT(UNIT, str)) OK({});
}
