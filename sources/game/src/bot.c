#include <game/bot.h>
#include <game/command.h>


// MARK: - commands
static const struct {
  str         cmd;
  cmd_error (*eval)(bot *self, str from, priv_command *cmd);
} priv_cmd_hdlrs[] = {
  { str_literal("list"), priv_command_list },
};

static const struct {
  str         cmd;
  cmd_error (*eval)(bot *self, str channel, str from, game_command *cmd);
} game_cmd_hdlrs[] = {
  { str_literal("query"), game_command_query },
};

static constexpr usize game_cmd_hdlr_count = sizeof(game_cmd_hdlrs) / sizeof(game_cmd_hdlrs[0]);
static constexpr usize priv_cmd_hdlr_count = sizeof(priv_cmd_hdlrs) / sizeof(priv_cmd_hdlrs[0]);


static RESULT(UNIT, str) bot__eval_priv_command(bot *self, str from, str message) {
  assert(self != NULL);
  assert(from.data    != NULL && from.length    > 0);
  assert(message.data != NULL && message.length > 0);

  priv_command cmd = {};
  if (priv_command_parse(&cmd, message)) {
    for (usize i = 0; i < priv_cmd_hdlr_count; ++i) {
      typeof(priv_cmd_hdlrs[i]) h = priv_cmd_hdlrs[i];

      if (str_equal(cmd.type, h.cmd)) {
        return h.eval(self, from, &cmd);
      }
    }

    return priv_command_unknown(self, from);
  }

  return (RESULT(UNIT, str)){
    .is_ok = true,
  };
}


static RESULT(UNIT, str) bot__eval_game_command(bot *self, str channel, str from, str message) {
  assert(self != NULL);
  assert(channel.data != NULL && channel.length > 0);
  assert(from.data    != NULL && from.length    > 0);
  assert(message.data != NULL && message.length > 0);

  game_command cmd = {};
  if (game_command_parse(&cmd, message)) {
    for (usize i = 0; i < game_cmd_hdlr_count; ++i) {
      typeof(game_cmd_hdlrs[i]) h = game_cmd_hdlrs[i];

      if (str_equal(cmd.type, h.cmd)) {
        return h.eval(self, channel, from, &cmd);
      }
    }

    return game_command_unknown(self, channel, from);
  }

  return (RESULT(UNIT, str)){
    .is_ok = true,
  };
}


// MARK: - handlers
static irc_error bot__ping(void *udata, irc_msg *msg) {
  assert(udata != NULL);
  assert(msg   != NULL);

  bot *self = (bot *)udata;

  RESULT(UNIT, conn_error) res = conn_write(self->conn, str_literal("PONG"));
  if (!res.is_ok) {
    return (irc_error){
      .is_ok = false,
      .err   = strview_from_cstr(conn_strerror(res.err)),
    };
  }

  return (irc_error){
    .is_ok = true,
  };
}


static irc_error bot__privmsg(void *udata, irc_msg *msg) {
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

  return (irc_error){
    .is_ok = true,
  };
}


static irc_error bot__fallback(void *udata, irc_msg *msg) {
  assert(udata != NULL);
  assert(msg   != NULL);

  allocator a = std_allocator();
  str       s = irc_msg_encode(msg, a);

  printf("%.*s", (int)s.length, s.data);

  str_free(a, &s);

  return (irc_error){
    .is_ok = true,
  };
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

  str s_nick = irc_msg_encode(&m_nick, a);
  RESULT(UNIT, conn_error) res = conn_write(self->conn, s_nick);
  str_free(a, &s_nick);
  if (!res.is_ok) {
    return (RESULT(UNIT, str)){
      .is_ok = false,
      .err   = strview_from_cstr(conn_strerror(res.err)),
    };
  }

  irc_msg m_user     = {};
  m_user.has_prefix  = false;
  m_user.command     = str_literal("USER");
  m_user.param_count = 3;
  m_user.params[0]   = self->nick;
  m_user.params[1]   = str_literal("0");
  m_user.params[2]   = str_literal("*");
  m_user.trailing    = str_literal("mudirc bot");

  str s_user = irc_msg_encode(&m_user, a);
  res = conn_write(self->conn, s_user);
  str_free(a, &s_user);
  if (!res.is_ok) {
    return (RESULT(UNIT, str)){
      .is_ok = false,
      .err   = strview_from_cstr(conn_strerror(res.err)),
    };
  }

  return (RESULT(UNIT, str)){
    .is_ok = true,
  };
}


// MARK: - api
RESULT(UNIT, str) bot_init(bot *self, const char *dbpath, conn_ref c, str nick) {
  assert(self   != NULL);
  assert(dbpath != NULL);
  assert(c      != NULL);
  assert(nick.data != NULL && nick.length > 0);

  int res = sqlite3_open(dbpath, &self->db);
  assert_release(self->db != NULL);
  if (res != SQLITE_OK) {
    return (RESULT(UNIT, str)){
      .is_ok = false,
      .err   = strview_from_cstr(sqlite3_errmsg(self->db)),
    };
  }

  self->conn = c;
  self->nick = nick;

  return bot__auth(self);
}


void bot_deinit(bot *self) {
  assert(self != NULL);

  if (self->db != NULL) {
    sqlite3_close(self->db);
  }

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
