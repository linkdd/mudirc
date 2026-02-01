#include <preludec/net/lib.h>
#include <preludec/mem/arena.h>

#include <game/core/event-loop.h>
#include <game/appstate.h>


int main(int argc, char *argv[static argc]) {
  if (argc < 5) {
    fprintf(stderr, "Usage: %s <db path> <server> <port> <nick>\n", argv[0]);
    return EXIT_FAILURE;
  }

  appconfig config = {
    .dbpath = argv[1],
    .server = argv[2],
    .port   = argv[3],
    .nick   = argv[4],
  };
  appstate state = {};

  int exit_status = EXIT_SUCCESS;

  if (appstate_init(&state, &config)) {
    auto res = event_loop(&state.bot);
    if (!res.is_ok) {
      fprintf(
        stderr, "ERROR: %.*s\n",
        (int)res.err.length, res.err.data
      );
      exit_status = EXIT_FAILURE;
    }

    fprintf(stderr, "INFO: Shutting down\n");
  }
  else {
    exit_status = EXIT_FAILURE;
  }

  appstate_deinit(&state);

  return exit_status;
}
