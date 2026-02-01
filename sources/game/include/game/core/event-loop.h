#pragma once

#include <preludec/defs.h>
#include <preludec/mem/str.h>
#include <preludec/net/conn.h>

#include <game/core/bot.h>


RESULT(UNIT, str) event_loop(bot *b);
