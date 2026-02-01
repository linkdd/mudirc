#include <game/core/bot.h>
#include <game/core/command.h>


// MARK: - commands
static const struct {
  str         cmd;
  cmd_result (*eval)(bot *self, str from, priv_command *cmd);
} priv_cmd_hdlrs[] = {
  { str_literal("list"), priv_command_list },
};

static const struct {
  str         cmd;
  cmd_result (*eval)(bot *self, str channel, str from, game_command *cmd);
} game_cmd_hdlrs[] = {
  { str_literal("query"), game_command_query },
};

static constexpr usize game_cmd_hdlr_count = ARRAY_COUNT(game_cmd_hdlrs);
static constexpr usize priv_cmd_hdlr_count = ARRAY_COUNT(priv_cmd_hdlrs);


static RESULT(UNIT, str) bot__eval_priv_command(bot *self, str from, str message) {
  assert(self != NULL);
  assert(from.data    != NULL && from.length    > 0);
  assert(message.data != NULL && message.length > 0);

  priv_command cmd = {};
  if (priv_command_parse(&cmd, message)) {
    for (usize i = 0; i < priv_cmd_hdlr_count; ++i) {
      auto h = priv_cmd_hdlrs[i];

      if (str_equal(cmd.type, h.cmd)) {
        return h.eval(self, from, &cmd);
      }
    }

    return priv_command_unknown(self, from);
  }

  return (RESULT(UNIT, str)) OK({});
}


static RESULT(UNIT, str) bot__eval_game_command(bot *self, str channel, str from, str message) {
  assert(self != NULL);
  assert(channel.data != NULL && channel.length > 0);
  assert(from.data    != NULL && from.length    > 0);
  assert(message.data != NULL && message.length > 0);

  game_command cmd = {};
  if (game_command_parse(&cmd, message)) {
    for (usize i = 0; i < game_cmd_hdlr_count; ++i) {
      auto h = game_cmd_hdlrs[i];

      if (str_equal(cmd.type, h.cmd)) {
        return h.eval(self, channel, from, &cmd);
      }
    }

    return game_command_unknown(self, channel, from);
  }

  return (RESULT(UNIT, str)) OK({});
}


// MARK: - handlers
static irc_result bot__ping(void *udata, irc_msg *msg) {
  assert(udata != NULL);
  assert(msg   != NULL);

  bot *self = (bot *)udata;

  irc_msg pong     = {};
  pong.has_prefix  = false;
  pong.command     = str_literal("PONG");
  pong.trailing    = msg->trailing;
  pong.param_count = msg->param_count;

  for (usize i = 0; i < msg->param_count; ++i) {
    pong.params[i] = msg->params[i];
  }

  auto res = irc_msg_send(&pong, self->conn, std_allocator());
  if (!res.is_ok) {
    return (irc_result) ERR(strview_from_cstr(conn_strerror(res.err)));
  }

  return (irc_result) OK({});
}


static irc_result bot__privmsg(void *udata, irc_msg *msg) {
  assert(udata != NULL);
  assert(msg   != NULL);

  bot *self = (bot *)udata;

  if (msg->has_prefix && msg->prefix.is_server == false) {
    str from = msg->prefix.user.nick;
    str to   = msg->params[0];

    if (str_equal(to, self->nick)) {
      bot__eval_priv_command(self, from, msg->trailing);
    }
    else {
      bot__eval_game_command(self, to, from, msg->trailing);
    }
  }

  return (irc_result) OK({});
}


static irc_result bot__fallback(void *udata, irc_msg *msg) {
  assert(udata != NULL);
  assert(msg   != NULL);

  allocator a = std_allocator();
  str       s = irc_msg_encode(msg, a);

  printf("%.*s", (int)s.length, s.data);

  str_free(a, &s);

  return (irc_result) OK({});
}


// MARK: - auth
RESULT(UNIT, str) bot__auth(bot *self) {
  assert(self != NULL);

  allocator a = std_allocator();

  irc_msg m_nick     = {};
  m_nick.has_prefix  = false;
  m_nick.command     = str_literal("NICK");
  m_nick.param_count = 1;
  m_nick.params[0]   = self->nick;

  auto res = irc_msg_send(&m_nick, self->conn, a);
  if (!res.is_ok) {
    return (RESULT(UNIT, str)) ERR(strview_from_cstr(conn_strerror(res.err)));
  }

  irc_msg m_user     = {};
  m_user.has_prefix  = false;
  m_user.command     = str_literal("USER");
  m_user.param_count = 3;
  m_user.params[0]   = self->nick;
  m_user.params[1]   = str_literal("0");
  m_user.params[2]   = str_literal("*");
  m_user.trailing    = str_literal("mudirc bot");

  res = irc_msg_send(&m_user, self->conn, a);
  if (!res.is_ok) {
    return (RESULT(UNIT, str)) ERR(strview_from_cstr(conn_strerror(res.err)));
  }

  return (RESULT(UNIT, str)) OK({});
}


// MARK: - api
RESULT(UNIT, str) bot_init(bot *self, dbconn *db, conn_ref c, str nick) {
  assert(self != NULL);
  assert(db    != NULL);
  assert(c    != NULL);
  assert(nick.data != NULL && nick.length > 0);

  self->db   = db;
  self->conn = c;
  self->nick = nick;

  return bot__auth(self);
}


void bot_deinit(bot *self) {
  assert(self != NULL);

  memset(self, 0, sizeof(bot));
}


irc_handler bot_handler(bot *self) {
  assert(self != NULL);

  return (irc_handler){
    .ping    = bot__ping,
    .privmsg = bot__privmsg,

    .fallback = bot__fallback,

    .udata = self,
  };
}
