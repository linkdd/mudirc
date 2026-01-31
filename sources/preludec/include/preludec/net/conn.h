#pragma once

#include <preludec/defs.h>
#include <preludec/mem/alloc.h>
#include <preludec/mem/span.h>

#include <preludec/net/socket.h>


typedef struct conn conn;
struct conn {
  socket_handle sock;
};

typedef conn* conn_ref;

typedef enum {
  CONN_ERR_WRITE_FAILED,
  CONN_ERR_READ_FAILED,
  CONN_ERR_CLOSED,
} conn_error;


void conn_init  (conn *self, socket_handle sock);
void conn_deinit(conn *self);

RESULT(UNIT, conn_error) conn_write(conn *self, const_span payload);
RESULT(UNIT, conn_error) conn_read (conn *self, span       buffer);

const char *conn_strerror(conn_error err);
