#include <preludec/net/lib.h>
#include <preludec/mem/arena.h>

#include <game/event-loop.h>
#include <game/lifecycle.h>
#include <game/bot.h>


static void on_shutdown_request(void *ctx) {
  conn_ref c = (conn_ref)ctx;

  char      buf[irc_msg_buffer_size] = {};
  arena     scratch                  = {};
  allocator a                        = arena_allocator(&scratch);
  arena_init(&scratch, make_span(buf, irc_msg_buffer_size));

  irc_msg m_quit     = {};
  m_quit.has_prefix  = false;
  m_quit.command     = str_literal("QUIT");
  m_quit.param_count = 0;
  m_quit.trailing    = str_literal("He's dead Jim.");

  str s_quit = irc_msg_encode(&m_quit, a);
  (void)conn_write(c, s_quit);

  conn_shutdown(c);
}


int main(int argc, char *argv[static argc]) {
  int exit_status = EXIT_SUCCESS;

  if (argc < 5) {
    fprintf(stderr, "Usage: %s <db path> <server> <port> <nick>\n", argv[0]);
    exit_status = EXIT_FAILURE;
    goto err0;
  }

  const char *dbpath = argv[1];
  const char *server = argv[2];
  const char *port   = argv[3];
  const char *nick   = argv[4];

  netlib_init();

  fprintf(stderr, "INFO: Connecting to %s:%s...\n", server, port);
  RESULT(conn, str) netlib_res = netlib_create_tcp_client(server, port);
  if (!netlib_res.is_ok) {
    fprintf(
      stderr, "ERROR: Connection to %s:%s failed: %.*s\n",
      server, port,
      (int)netlib_res.err.length, netlib_res.err.data
    );
    exit_status = EXIT_FAILURE;
    goto err1;
  }

  conn_ref c = &netlib_res.ok;
  lc_init(c, on_shutdown_request);

  bot b = {};
  RESULT(UNIT, str) bot_res = bot_init(&b, dbpath, c, strview_from_cstr(nick));
  if (!bot_res.is_ok) {
    fprintf(
      stderr, "ERROR: %.*s\n",
      (int)bot_res.err.length, bot_res.err.data
    );
    exit_status = EXIT_FAILURE;
    goto err2;
  }

  RESULT(UNIT, str) loop_res = event_loop(&b);
  if (!loop_res.is_ok) {
    fprintf(
      stderr, "ERROR: %.*s\n",
      (int)loop_res.err.length, loop_res.err.data
    );
    exit_status = EXIT_FAILURE;
    goto err3;
  }

  fprintf(stderr, "INFO: Shutting down\n");

err3:
  bot_deinit(&b);

err2:
  conn_deinit(c);

err1:
  netlib_deinit();

err0:
  return exit_status;
}
