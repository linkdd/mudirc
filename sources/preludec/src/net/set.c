#include <preludec/net/set.h>


VEC_IMPLEMENTATION(conn, conn_ref);


void conn_set_init(conn_set *self, allocator a) {
  assert(self != NULL);

  vec_conn_init(&self->conns, a, 4);
}


void conn_set_deinit(conn_set *self) {
  assert(self != NULL);

  vec_conn_deinit(&self->conns);
  memset(self, 0, sizeof(conn_set));
}


void conn_set_add(conn_set *self, conn_ref c) {
  assert(self != NULL);
  assert(c    != NULL);

  vec_conn_push(&self->conns, c);
}


RESULT(conn_ref, conn_set_error) conn_set_wait(conn_set *self) {
  assert(self != NULL);

  if (self->conns.count == 0) {
    return (RESULT(conn_ref, conn_set_error)){
      .is_ok = true,
      .ok    = NULL,
    };
  }

  fd_set fds = {};
  FD_ZERO(&fds);

  socket_handle maxfd = 0;

  for (usize i = 0; i < self->conns.count; ++i) {
    conn_ref c = *vec_conn_at(&self->conns, i);
    FD_SET(c->sock, &fds);

    if (c->sock > maxfd) {
      maxfd = c->sock;
    }
  }

  int res = select(maxfd + 1, &fds, NULL, NULL, NULL);
  if (res > 0) {
    for (usize i = 0; i < self->conns.count; ++i) {
      conn_ref c = *vec_conn_at(&self->conns, i);

      if (FD_ISSET(c->sock, &fds)) {
        return (RESULT(conn_ref, conn_set_error)){
          .is_ok = true,
          .ok    = c,
        };
      }
    }

    unreachable();
  }

  return (RESULT(conn_ref, conn_set_error)){
    .is_ok = false,
    .err   = CONN_SET_ERR_SELECT_FAILED,
  };
}


const char *conn_set_strerror(conn_set_error err) {
  switch (err) {
    case CONN_SET_ERR_SELECT_FAILED:
      return "select() failed";

      default:
      return "unknown error";
  }
}
