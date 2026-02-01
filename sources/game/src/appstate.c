#include <game/appstate.h>


extern const initializer i_netlib;
extern const initializer i_database;
extern const initializer i_conn;
extern const initializer i_lifecycle;
extern const initializer i_bot;


static const initializer *g_initializers[] = {
  &i_netlib,
  &i_database,
  &i_conn,
  &i_lifecycle,
  &i_bot,
};

static const usize g_initializer_count = ARRAY_COUNT(g_initializers);
static usize       g_initializer_done  = 0;


bool appstate_init(appstate *state, const appconfig *config) {
  assert(state  != NULL);
  assert(config != NULL);

  for (usize i = 0; i < g_initializer_count; ++i) {
    const initializer *initializer = g_initializers[i];

    bool res = initializer->setup(state, config);
    if (!res) {
      return false;
    }

    g_initializer_done++;
  }

  return true;
}


void appstate_deinit(appstate *state) {
  assert(state != NULL);

  for (usize i = g_initializer_done; i > 0; --i) {
    const initializer *initializer = g_initializers[i - 1];
    initializer->teardown(state);
  }
}
