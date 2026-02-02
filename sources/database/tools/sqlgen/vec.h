#pragma once

#include <preludec/defs.h>
#include <preludec/mem/str.h>
#include <preludec/collections/vec.h>


typedef struct sql_param sql_param;
struct sql_param {
  str name;
  str type;
};

VEC_DEFINE(str,       str);
VEC_DEFINE(sql_param, sql_param);
