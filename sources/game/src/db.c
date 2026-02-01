#include <game/db.h>


RESULT(UNIT, str) database_init(database *self, const char *path) {
  assert(self != NULL);
  assert(path != NULL);

  int res = sqlite3_open(path, &self->handle);
  assert_release(self->handle != NULL);
  if (res != SQLITE_OK) {
    return (RESULT(UNIT, str)) ERR(strview_from_cstr(sqlite3_errmsg(self->handle)));
  }

  return (RESULT(UNIT, str)) OK({});
}


void database_deinit(database *self) {
  assert(self != NULL);

  if (self->handle != NULL) {
    sqlite3_close(self->handle);
  }

  memset(self, 0, sizeof(database));
}
