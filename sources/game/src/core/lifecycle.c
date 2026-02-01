#include <signal.h>

#include <game/core/lifecycle.h>


atomic_bool g_running = true;

static void                *g_ctx              = NULL;
static lc_shutdown_request  g_shutdown_request = NULL;

static void handle_sigint(int signum) {
  (void)signum;
  atomic_store(&g_running, false);
  g_shutdown_request(g_ctx);
  fprintf(stderr, "INFO: Shutdown requested\n");
}


void lc_init(void *ctx, lc_shutdown_request shutdown_request) {
  g_ctx              = ctx;
  g_shutdown_request = shutdown_request;
  signal(SIGINT, handle_sigint);
}


bool lc_running(void) {
  return atomic_load(&g_running);
}
