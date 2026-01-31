#include <preludec/net/conn.h>


void conn_init(conn *self, socket_handle sock) {
  assert(self != NULL);
  assert(sock != invalid_socket);

  self->sock = sock;
}


void conn_deinit(conn *self) {
  assert(self != NULL);

  netlib_socket_close(self->sock);
  memset(self, 0, sizeof(conn));
}


conn_error conn_write(conn *self, const_span payload) {
  assert(self != NULL);
  assert(payload.data != NULL && payload.size > 0);

  usize sent = 0;
  while (sent < payload.size) {
    void *begin = (void*)((uptr)payload.data + sent);
    usize left  = payload.size - sent;

    isize n = send(self->sock, begin, left, 0);
    if (n < 0) {
      return CONN_ERR_WRITE_FAILED;
    }

    sent += n;
  }

  return CONN_ERR_NONE;
}


conn_error conn_read(conn *self, span buffer) {
  assert(self != NULL);
  assert(buffer.data != NULL && buffer.size > 0);

  usize received = 0;
  while (received < buffer.size) {
    void *begin = (void*)((uptr)buffer.data + received);
    usize left  = buffer.size - received;

    isize n = recv(self->sock, begin, left, 0);
    if (n < 0) {
      return CONN_ERR_READ_FAILED;
    }
    if (n == 0) {
      return CONN_ERR_CLOSED;
    }

    received += n;
  }

  return CONN_ERR_NONE;
}
