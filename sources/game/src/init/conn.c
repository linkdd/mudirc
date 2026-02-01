#include <game/appstate.h>

#include <preludec/net/lib.h>


static bool i_conn_setup(appstate *state, const appconfig *config) {
  assert(state  != NULL);
  assert(config != NULL);

  fprintf(stderr, "INFO: Connecting to %s:%s...\n", config->server, config->port);
  auto res = netlib_create_tcp_client(config->server, config->port);
  if (!res.is_ok) {
    fprintf(
      stderr, "ERROR: Connection to %s:%s failed: %.*s\n",
      config->server, config->port,
      (int)res.err.length, res.err.data
    );
    return false;
  }

  state->conn = res.ok;

  return true;
}


static void i_conn_teardown(appstate *state) {
  assert(state != NULL);

  conn_deinit(&state->conn);
}


const initializer i_conn = {
  .setup    = i_conn_setup,
  .teardown = i_conn_teardown,
};
