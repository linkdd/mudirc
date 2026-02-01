#pragma once

#include <preludec/defs.h>

#include <game/core/event-loop.h>
#include <game/core/lifecycle.h>
#include <game/core/bot.h>


typedef struct appconfig appconfig;
struct appconfig {
  char *dbpath;
  char *server;
  char *port;
  char *nick;
};

typedef struct appstate appstate;
struct appstate {
  dbconn db;
  conn   conn;
  bot    bot;
};

typedef struct initializer initializer;
struct initializer {
  bool (*setup)   (appstate *state, const appconfig *config);
  void (*teardown)(appstate *state);
};


bool appstate_init  (appstate *state, const appconfig *config);
void appstate_deinit(appstate *state);
