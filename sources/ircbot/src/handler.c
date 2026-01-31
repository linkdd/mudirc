#include <ircbot/handler.h>


irc_error irc_handler_call(irc_handler self, irc_msg *msg) {
  assert(msg  != NULL);

  struct {
    str cmd;
    irc_error (*handler)(void *udata, irc_msg *msg);
  } handlers[] = {
    { str_literal("001"),     self.welcome   },
    { str_literal("433"),     self.nickinuse },
    { str_literal("PING"),    self.ping      },
    { str_literal("PRIVMSG"), self.privmsg   },
    { str_literal("NOTICE"),  self.notice    },
    { str_literal("JOIN"),    self.join      },
    { str_literal("PART"),    self.part      },
    { str_literal("QUIT"),    self.quit      },
    { str_literal("NICK"),    self.nick      },
    { str_literal("MODE"),    self.mode      },
  };

  const usize handler_count = sizeof(handlers) / sizeof(handlers[0]);

  for (usize i = 0; i < handler_count; ++i) {
    typeof(handlers[i]) h = handlers[i];

    if (str_equal(msg->command, h.cmd)) {
      if (h.handler != NULL) {
        return h.handler(self.udata, msg);
      }

      break;
    }
  }

  if (self.fallback != NULL) {
    return self.fallback(self.udata, msg);
  }

  return (irc_error){
    .is_ok = true,
  };
}
