#include <game/bot.h>

// MARK: - handlers
static RESULT(UNIT, str) bot__ping(void *udata, irc_msg *msg) {
  assert(udata != NULL);
  assert(msg   != NULL);

  bot *self = (bot *)udata;

  RESULT(UNIT, conn_error) res = conn_write(self->conn, str_literal("PONG"));
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


static RESULT(UNIT, str) bot__fallback(void *udata, irc_msg *msg) {
  assert(udata != NULL);
  assert(msg   != NULL);

  allocator a = std_allocator();
  str       s = irc_msg_encode(msg, a);

  printf("%.*s", (int)s.length, s.data);

  str_free(a, &s);

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

  return (RESULT(UNIT, str)){
    .is_ok = true,
  };
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
    .ping = bot__ping,

    .fallback = bot__fallback,

    .udata = self,
  };
}
