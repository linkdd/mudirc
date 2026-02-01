#include <game/appstate.h>

#include <preludec/mem/arena.h>


static void on_shutdown_request(void *ctx) {
  appstate *state = (appstate *)ctx;

  char      buf[irc_msg_buffer_size] = {};
  arena     scratch                  = {};
  allocator a                        = arena_allocator(&scratch);
  arena_init(&scratch, make_span(buf, irc_msg_buffer_size));

  irc_msg quit     = {};
  quit.has_prefix  = false;
  quit.command     = str_literal("QUIT");
  quit.param_count = 0;
  quit.trailing    = str_literal("He's dead Jim.");

  (void)irc_msg_send(&quit, &state->conn, a);

  conn_shutdown(&state->conn);
}


static bool i_lifecycle_setup(appstate *state, const appconfig *config) {
  assert(state  != NULL);
  (void)config;

  lc_init(state, on_shutdown_request);

  return true;
}


static void i_lifecycle_teardown(appstate *state) {
  (void)state;
}


const initializer i_lifecycle = {
  .setup    = i_lifecycle_setup,
  .teardown = i_lifecycle_teardown,
};
