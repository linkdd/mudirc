#pragma once

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #define NOMINMAX
  #include <winsock2.h>
  #include <ws2tcpip.h>


  typedef SOCKET socket_handle;
  typedef int    socklen_t;

  static constexpr socket_handle invalid_socket = INVALID_SOCKET;


  static inline void netlib_socket_close(socket_handle sock) {
    if (sock != invalid_socket) {
      closesocket(sock);
    }
  }
#else
  #include <sys/socket.h>
  #include <sys/select.h>
  #include <sys/types.h>
  #include <sys/time.h>

  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netdb.h>

  #include <unistd.h>


  typedef int socket_handle;

  static constexpr socket_handle invalid_socket = -1;


  static inline void netlib_socket_close(socket_handle sock) {
    if (sock != invalid_socket) {
      close(sock);
    }
  }
#endif
