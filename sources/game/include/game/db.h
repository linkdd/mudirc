#pragma once

#include <preludec/defs.h>
#include <preludec/mem/str.h>

#include <sqlite3.h>


typedef struct database database;
struct database {
  sqlite3 *handle;
};


RESULT(UNIT, str) database_init  (database *self, const char *path);
void              database_deinit(database *self);
