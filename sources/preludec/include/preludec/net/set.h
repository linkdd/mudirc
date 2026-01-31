#pragma once

#include <preludec/defs.h>
#include <preludec/collections/vec.h>
#include <preludec/net/conn.h>


VEC_DEFINE(conn, conn_ref);


typedef struct conn_set conn_set;
struct conn_set {
  VEC(conn) conns;
};

typedef enum {
  CONN_SET_ERR_SELECT_FAILED,
} conn_set_error;


void conn_set_init  (conn_set *self, allocator a);
void conn_set_deinit(conn_set *self);

void                             conn_set_add (conn_set *self, conn_ref c);
RESULT(conn_ref, conn_set_error) conn_set_wait(conn_set *self);
