#pragma once

#include <preludec/defs.h>
#include <preludec/mem/alloc.h>
#include <preludec/collections/vec.h>
#include <preludec/net/conn.h>
#include <preludec/sync/coro.h>


VEC_DEFINE(conn, conn*);


typedef struct conn_set conn_set;
struct conn_set {
  VEC(conn) conns;
};


void conn_set_init  (conn_set *self, allocator a);
void conn_set_deinit(conn_set *self);

void  conn_set_add (conn_set *self, conn *c);
coro *conn_set_wait(conn_set *self, allocator a, conn **c);
