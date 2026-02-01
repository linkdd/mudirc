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

  irc_msg quit     = {};
  quit.has_prefix  = false;
  quit.command     = str_literal("QUIT");
  quit.param_count = 0;
  quit.trailing    = str_literal("He's dead Jim.");

  (void)irc_msg_send(&quit, c, a);

  conn_shutdown(c);
}


int main(int argc, char *argv[static argc]) {
  int exit_status = EXIT_SUCCESS;

  netlib_init();

  if (argc < 5) {
    fprintf(stderr, "Usage: %s <db path> <server> <port> <nick>\n", argv[0]);
    exit_status = EXIT_FAILURE;
    goto err0;
  }

  const char *dbpath = argv[1];
  const char *server = argv[2];
  const char *port   = argv[3];
  const char *nick   = argv[4];

  fprintf(stderr, "INFO: Opening database at %s...\n", dbpath);
  database db = {};
  auto db_res = database_init(&db, dbpath);
  if (!db_res.is_ok) {
    fprintf(
      stderr, "ERROR: Failed to open database at %s: %.*s\n",
      dbpath,
      (int)db_res.err.length, db_res.err.data
    );
    exit_status = EXIT_FAILURE;
    goto err1;
  }

  fprintf(stderr, "INFO: Connecting to %s:%s...\n", server, port);
  auto netlib_res = netlib_create_tcp_client(server, port);
  if (!netlib_res.is_ok) {
    fprintf(
      stderr, "ERROR: Connection to %s:%s failed: %.*s\n",
      server, port,
      (int)netlib_res.err.length, netlib_res.err.data
    );
    exit_status = EXIT_FAILURE;
    goto err2;
  }

  conn_ref c = &netlib_res.ok;
  lc_init(c, on_shutdown_request);

  bot b = {};
  auto bot_res = bot_init(&b, &db, c, strview_from_cstr(nick));
  if (!bot_res.is_ok) {
    fprintf(
      stderr, "ERROR: %.*s\n",
      (int)bot_res.err.length, bot_res.err.data
    );
    exit_status = EXIT_FAILURE;
    goto err3;
  }

  auto loop_res = event_loop(&b);
  if (!loop_res.is_ok) {
    fprintf(
      stderr, "ERROR: %.*s\n",
      (int)loop_res.err.length, loop_res.err.data
    );
    exit_status = EXIT_FAILURE;
    goto err4;
  }

  fprintf(stderr, "INFO: Shutting down\n");

err4:
  bot_deinit(&b);

err3:
  conn_deinit(c);

err2:
err1:
  database_deinit(&db);

err0:
  netlib_deinit();
  return exit_status;
}
