#include <game/command.h>


static str skip_spaces(str line) {
  usize i = 0;
  while (i < line.length && line.data[i] == ' ') {
    i++;
  }

  return str_slice(line, i, line.length);
}


static str parse_command(str *type, str line) {
  usize i = 0;
  while (i < line.length && line.data[i] != ' ') {
    i++;
  }

  *type = str_slice(line, 0, i);

  return str_slice(line, i + 1, line.length);
}


static str parse_param(str *param, str line) {
  usize i = 0;
  while (i < line.length && line.data[i] != ' ') {
    i++;
  }

  *param = str_slice(line, 0, i);

  return str_slice(line, i + 1, line.length);
}


bool priv_command_parse(priv_command *self, str line) {
  assert(self != NULL);
  assert(line.data != NULL && line.length > 0);

  str line_nocommand = skip_spaces(parse_command(&self->type, line));
  if (line_nocommand.length == 0) {
    self->param_count = 0;
    return true;
  }

  str line_nextparam = line_nocommand;
  for (usize param_idx = 0; param_idx < priv_command_max_params; ++param_idx) {
    line_nextparam = skip_spaces(line_nextparam);
    if (line_nextparam.length == 0) break;

    line_nextparam = parse_param(&self->params[param_idx], line_nextparam);
    self->param_count++;
  }

  return true;
}


bool game_command_parse(game_command *self, str line) {
  assert(self != NULL);
  assert(line.data != NULL && line.length > 0);

  if (line.data[0] != '!') return false;
  line = str_slice(line, 1, line.length);

  str line_nocommand = skip_spaces(parse_command(&self->type, line));
  if (line_nocommand.length == 0) {
    self->param_count = 0;
    return true;
  }

  str line_nextparam = line_nocommand;
  for (usize param_idx = 0; param_idx < game_command_max_params; ++param_idx) {
    line_nextparam = skip_spaces(line_nextparam);
    if (line_nextparam.length == 0) break;

    line_nextparam = parse_param(&self->params[param_idx], line_nextparam);
    self->param_count++;
  }

  return true;
}
