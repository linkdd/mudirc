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


RESULT(UNIT, conn_error) conn_write(conn *self, str payload) {
  assert(self != NULL);
  assert(payload.data != NULL && payload.length > 0);

  usize sent = 0;
  while (sent < payload.length) {
    void *begin = (void*)((uptr)payload.data + sent);
    usize left  = payload.length - sent;

    isize n = send(self->sock, begin, left, 0);
    if (n < 0) {
      return (RESULT(UNIT, conn_error)){
        .is_ok = false,
        .err   = CONN_ERR_WRITE_FAILED,
      };
    }

    sent += n;
  }

  return (RESULT(UNIT, conn_error)){
    .is_ok = true,
  };
}


RESULT(UNIT, conn_error) conn_read(conn *self, str *buffer) {
  assert(self != NULL);
  assert(buffer->data != NULL && buffer->capacity > 0);

  isize n = recv(self->sock, buffer->data, buffer->capacity, 0);
  if (n < 0) {
    return (RESULT(UNIT, conn_error)){
      .is_ok = false,
      .err   = CONN_ERR_READ_FAILED,
    };
  }
  if (n == 0) {
    return (RESULT(UNIT, conn_error)){
      .is_ok = false,
      .err   = CONN_ERR_CLOSED,
    };
  }

  buffer->length = (usize)n;

  return (RESULT(UNIT, conn_error)){
    .is_ok = true,
  };
}


const char *conn_strerror(conn_error err) {
  switch (err) {
    case CONN_ERR_WRITE_FAILED:
      return "send() failed";

      case CONN_ERR_READ_FAILED:
      return "recv() failed";

    case CONN_ERR_CLOSED:
      return "connection closed";

    default:
      return "unknown error";
  }
}
