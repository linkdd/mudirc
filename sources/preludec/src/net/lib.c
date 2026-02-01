#include <preludec/net/lib.h>

#include <errno.h>


void netlib_init(void) {
#ifdef _WIN32
  WSADATA wsaData;
  assert_release(0 == WSAStartup(MAKEWORD(2, 2), &wsaData));
#endif
}


void netlib_deinit(void) {
#ifdef _WIN32
  WSACleanup();
#endif
}


RESULT(conn, str) netlib_create_tcp_client(const char *host, const char *port) {
  struct addrinfo hints = {};
  hints.ai_family       = AF_UNSPEC;
  hints.ai_socktype     = SOCK_STREAM;
  hints.ai_protocol     = IPPROTO_TCP;

  struct addrinfo *result = NULL;
  if (getaddrinfo(host, port, &hints, &result) != 0) {
    return (RESULT(conn, str)) ERR(strview_from_cstr(gai_strerror(errno)));
  }

  struct addrinfo *p    = NULL;
  socket_handle    sock = invalid_socket;

  for (p = result; p != NULL; p = p->ai_next) {
    socket_handle s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (s == invalid_socket) {
      continue;
    }

    if (connect(s, p->ai_addr, p->ai_addrlen) == 0) {
      sock = s;
      break;
    }

    netlib_socket_close(s);
  }
  freeaddrinfo(result);

  if (p == NULL) {
    return (RESULT(conn, str)) ERR(str_literal("no resolved address"));
  }

  conn c = {};
  conn_init(&c, sock);
  return (RESULT(conn, str)) OK(c);
}
