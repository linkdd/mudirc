#include <preludec/net/set.h>


void conn_set_init(conn_set *self, allocator a) {
  assert(self != NULL);

  vec_conn_init(&self->conns, a, 4);
}


void conn_set_deinit(conn_set *self) {
  assert(self != NULL);

  vec_conn_deinit(&self->conns);
  memset(self, 0, sizeof(conn_set));
}


void conn_set_add(conn_set *self, conn *c) {
  assert(self != NULL);
  assert(c    != NULL);

  vec_conn_push(&self->conns, c);
}


typedef struct conn_set_waiter conn_set_waiter;
struct conn_set_waiter {
  conn_set  *cset;
  conn     **res;
};

static void conn_set__wait_task(coro *self, allocator a, span arg) {
  (void)a;

  conn_set_waiter w = {};
  memcpy(&w, arg.data, sizeof(conn_set_waiter));

  while (true) {
    while (w.cset->conns.count == 0) {
      coro_yield(self, NULL);
    }

    fd_set fds = {};
    FD_ZERO(&fds);

    socket_handle maxfd = 0;

    for (usize i = 0; i < w.cset->conns.count; ++i) {
      conn *c = *vec_conn_at(&w.cset->conns, i);
      FD_SET(c->sock, &fds);

      if (c->sock > maxfd) {
        maxfd = c->sock;
      }
    }

    struct timeval noblock = {0, 0};

    int res = select(maxfd + 1, &fds, NULL, NULL, &noblock);
    if (res > 0) {
      for (usize i = 0; i < w.cset->conns.count; ++i) {
        conn *c = *vec_conn_at(&w.cset->conns, i);

        if (FD_ISSET(c->sock, &fds)) {
          *w.res = c;
          return;
        }
      }
    }

    coro_yield(self, NULL);
  }
}


coro *conn_set_wait(conn_set *self, allocator a, conn **c) {
  assert(self != NULL);
  assert(c    != NULL);

  conn_set_waiter w = {
    .cset = self,
    .res  = c,
  };

  return make_coro(a, conn_set__wait_task, make_const_span(&w, sizeof(conn_set_waiter)));
}
