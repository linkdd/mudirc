#pragma once

#include <preludec/defs.h>
#include <preludec/mem/str.h>

#include <game/core/bot.h>


static constexpr usize priv_command_max_params = 4;
static constexpr usize game_command_max_params = 8;

typedef struct priv_command priv_command;
struct priv_command {
  str   type;
  usize param_count;
  str   params[priv_command_max_params];
};


typedef struct game_command game_command;
struct game_command {
  str   type;
  usize param_count;
  str   params[game_command_max_params];
};

typedef RESULT(UNIT, str) cmd_result;


bool priv_command_parse(priv_command *self, str line);
bool game_command_parse(game_command *self, str line);


cmd_result priv_command_list(bot *self, str from, priv_command *cmd);

cmd_result game_command_query(bot *self, str channel, str from, game_command *cmd);


cmd_result priv_command_unknown(bot *self, str from);
cmd_result game_command_unknown(bot *self, str channel, str from);
