#include <preludec/net/lib.h>

#include <game/event-loop.h>
#include <game/lifecycle.h>
#include <game/bot.h>


static void on_shutdown_request(void *ctx) {
  conn_ref c = (conn_ref)ctx;
  conn_shutdown(c);
}


static void on_terminate(void *ctx) {
  conn_ref c = *(conn_ref*)ctx;
  conn_deinit(c);
  fprintf(stderr, "INFO: Connection closed\n");
}


int main(int argc, char *argv[static argc]) {
  if (argc < 5) {
    fprintf(stderr, "Usage: %s <db path> <server> <port> <nick>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char *dbpath = argv[1];
  const char *server = argv[2];
  const char *port   = argv[3];
  const char *nick   = argv[4];

  netlib_init();
  atexit(netlib_deinit);

  fprintf(stderr, "INFO: Connecting to %s:%s...\n", server, port);
  RESULT(conn, str) netlib_res = netlib_create_tcp_client(server, port);
  if (!netlib_res.is_ok) {
    fprintf(
      stderr, "ERROR: Connection to %s:%s failed: %.*s\n",
      server, port,
      (int)netlib_res.err.length, netlib_res.err.data
    );
    return EXIT_FAILURE;
  }

  [[gnu::cleanup(on_terminate)]] conn_ref c = &netlib_res.ok;
  lc_init(c, on_shutdown_request);

  bot b = {};
  RESULT(UNIT, str) bot_res = bot_init(&b, dbpath, c, strview_from_cstr(nick));
  if (!bot_res.is_ok) {
    fprintf(
      stderr, "ERROR: %.*s\n",
      (int)bot_res.err.length, bot_res.err.data
    );
    return EXIT_FAILURE;
  }

  RESULT(UNIT, str) loop_res = event_loop(&b);

  bot_deinit(&b);

  if (!loop_res.is_ok) {
    fprintf(
      stderr, "ERROR: %.*s\n",
      (int)loop_res.err.length, loop_res.err.data
    );
    return EXIT_FAILURE;
  }

  fprintf(stderr, "INFO: Shutting down\n");

  return EXIT_SUCCESS;
}
