#pragma once

#include <preludec/defs.h>
#include <preludec/mem/str.h>

#include <sqlite3.h>


typedef struct dbconn dbconn;
struct dbconn {
  sqlite3 *handle;
};


RESULT(UNIT, str) dbconn_init  (dbconn *self, const char *path);
void              dbconn_deinit(dbconn *self);
