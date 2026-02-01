#include <ircbot/client.h>


void irc_client_init(irc_client *self, conn_ref conn, irc_handler handler) {
  assert(self != NULL);
  assert(conn != NULL);

  self->conn    = conn;
  self->handler = handler;

  self->buffer = (str){
    .data     = self->_bufmem,
    .length   = 0,
    .capacity = irc_client_buffer_size,
    .owned    = false,
  };
  memset(self->_bufmem, 0, irc_client_buffer_size);
}


RESULT(bool, str) irc_client_consume(irc_client *self) {
  assert(self != NULL);

  str view = str_slice(self->buffer, self->buffer.length, self->buffer.capacity);
  RESULT(UNIT, conn_error) c_res = conn_read(self->conn, &view);
  if (!c_res.is_ok) {
    if (c_res.err != CONN_ERR_CLOSED) {
      return (RESULT(bool, str)) ERR(strview_from_cstr(conn_strerror(c_res.err)));
    }
    else {
      return (RESULT(bool, str)) OK(false);
    }
  }
  self->buffer.length += view.length;

  do {
    usize end_of_message = 0;
    for (usize i = 0; i + 1 < self->buffer.length; ++i) {
      if (self->buffer.data[i] == '\r' && self->buffer.data[i + 1] == '\n') {
        end_of_message = i + 2;
        break;
      }
    }

    if (end_of_message == 0) {
      break;
    }

    str     line = str_slice(self->buffer, 0, end_of_message);
    irc_msg msg  = {};
    if (irc_msg_decode(&msg, line)) {
      RESULT(UNIT, str) res = irc_handler_call(self->handler, &msg);
      if (!res.is_ok) {
        return (RESULT(bool, str)) ERR(res.err);
      }
    }

    memmove(
      self->buffer.data,
      self->buffer.data + end_of_message,
      self->buffer.length - end_of_message
    );
    self->buffer.length -= end_of_message;
  } while (true);

  return (RESULT(bool, str)) OK(true);
}
