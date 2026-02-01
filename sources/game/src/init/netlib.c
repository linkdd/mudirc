#include <game/appstate.h>

#include <preludec/net/lib.h>


static bool i_netlib_setup(appstate *state, const appconfig *config) {
  (void)state;
  (void)config;

  netlib_init();

  return true;
}


static void i_netlib_teardown(appstate *state) {
  (void)state;

  netlib_deinit();
}


const initializer i_netlib = {
  .setup    = i_netlib_setup,
  .teardown = i_netlib_teardown,
};
