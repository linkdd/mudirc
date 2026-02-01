#include <game/appstate.h>


static bool i_database_setup(appstate *state, const appconfig *config) {
  assert(state  != NULL);
  assert(config != NULL);

  fprintf(stderr, "INFO: Opening database at %s...\n", config->dbpath);
  auto res = dbconn_init(&state->db, config->dbpath);
  if (!res.is_ok) {
    fprintf(
      stderr, "ERROR: Failed to open database at %s: %.*s\n",
      config->dbpath,
      (int)res.err.length, res.err.data
    );
    return false;
  }

  return true;
}


static void i_database_teardown(appstate *state) {
  assert(state != NULL);

  dbconn_deinit(&state->db);
}


const initializer i_database = {
  .setup    = i_database_setup,
  .teardown = i_database_teardown,
};
