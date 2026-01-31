#include <signal.h>

#include <game/lifecycle.h>


atomic_bool g_running = true;

static void *g_ctx                  = NULL;
static void (*g_destructor)(void *) = NULL;


static void handle_sigint(int signum) {
  (void)signum;
  atomic_store(&g_running, false);
  g_destructor(g_ctx);
}


void lc_init(void *ctx, void (*destructor)(void *)) {
  g_ctx        = ctx;
  g_destructor = destructor;
  signal(SIGINT, handle_sigint);
}


bool lc_running(void) {
  return atomic_load(&g_running);
}
