#include <game/database/conn.h>


RESULT(UNIT, str) dbconn_init(dbconn *self, const char *path) {
  assert(self != NULL);
  assert(path != NULL);

  int res = sqlite3_open(path, &self->handle);
  assert_release(self->handle != NULL);
  if (res != SQLITE_OK) {
    return (RESULT(UNIT, str)) ERR(strview_from_cstr(sqlite3_errmsg(self->handle)));
  }

  return (RESULT(UNIT, str)) OK({});
}


void dbconn_deinit(dbconn *self) {
  assert(self != NULL);

  if (self->handle != NULL) {
    sqlite3_close(self->handle);
  }

  memset(self, 0, sizeof(dbconn));
}
