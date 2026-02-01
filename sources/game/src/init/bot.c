#include <game/appstate.h>


static bool i_bot_setup(appstate *state, const appconfig *config) {
  assert(state  != NULL);
  assert(config != NULL);

  fprintf(stderr, "INFO: Initializing bot with nick %s...\n", config->nick);
  auto res = bot_init(&state->bot, &state->db, &state->conn, strview_from_cstr(config->nick));
  if (!res.is_ok) {
    fprintf(
      stderr, "ERROR: Failed to initialize bot: %.*s\n",
      (int)res.err.length, res.err.data
    );
    return false;
  }

  return true;
}


static void i_bot_teardown(appstate *state) {
  assert(state != NULL);

  bot_deinit(&state->bot);
}


const initializer i_bot = {
  .setup    = i_bot_setup,
  .teardown = i_bot_teardown,
};
