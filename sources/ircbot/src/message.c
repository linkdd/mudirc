#include <ircbot/message.h>


// MARK: - decoder
static str cut_crlf(str line) {
  usize len = line.length;
  if (len >= 2 && line.data[len - 2] == '\r' && line.data[len - 1] == '\n') {
    return str_slice(line, 0, len - 2);
  }
  else if (len >= 1 && (line.data[len - 1] == '\n' || line.data[len - 1] == '\r')) {
    return str_slice(line, 0, len - 1);
  }
  else {
    return line;
  }
}


static str skip_optional_tags(str line) {
  if (line.data[0] == '@') {
    usize i = 1;
    while (i < line.length && line.data[i] != ' ') {
      i++;
    }

    if (i < line.length) {
      return str_slice(line, i + 1, line.length);
    }
    else {
      return str_null();
    }
  }
  else {
    return line;
  }
}


static str skip_spaces(str line) {
  usize i = 0;
  while (i < line.length && line.data[i] == ' ') {
    i++;
  }

  return str_slice(line, i, line.length);
}


static str parse_prefix(irc_msg *self, str line) {
  if (line.data[0] == ':') {
    usize i = 1;
    while (i < line.length && line.data[i] != ' ') {
      i++;
    }

    str prefix = str_slice(line, 1, i);

    self->prefix.is_server    = true;
    self->prefix.server.ident = prefix;

    for (usize j = 0; j < prefix.length; ++j) {
      if (prefix.data[j] == '!') {
        self->prefix.is_server = false;
        self->prefix.user.nick = str_slice(prefix, 0, j);

        usize k = j + 1;
        while (k < prefix.length && prefix.data[k] != '@') {
          k++;
        }
        self->prefix.user.ident = str_slice(prefix, j + 1, k);
        self->prefix.user.host  = str_slice(prefix, k + 1, prefix.length);

        break;
      }
    }

    self->has_prefix = true;

    return str_slice(line, i + 1, line.length);
  }
  else {
    self->has_prefix = false;

    return line;
  }
}


static str parse_command(irc_msg *self, str line) {
  usize i = 0;
  while (i < line.length && line.data[i] != ' ') {
    i++;
  }

  self->command = str_slice(line, 0, i);

  return str_slice(line, i + 1, line.length);
}


static str parse_param(irc_msg *self, str line, usize param_idx) {
  usize i = 0;
  while (i < line.length && line.data[i] != ' ') {
    i++;
  }

  self->params[param_idx] = str_slice(line, 0, i);

  return str_slice(line, i + 1, line.length);
}


static bool is_trailing(str line) {
  return line.length > 0 && line.data[0] == ':';
}


bool irc_msg_decode(irc_msg *self, str line) {
  assert(self != NULL);
  assert(line.data != NULL && line.length > 0);

  str line_nocrlf = cut_crlf(line);
  if (line_nocrlf.length == 0) return false;

  str line_notags = skip_spaces(skip_optional_tags(line_nocrlf));
  if (line_notags.length == 0) return false;

  str line_noprefix = skip_spaces(parse_prefix(self, line_notags));
  if (line_noprefix.length == 0) return false;

  str line_nocommand = skip_spaces(parse_command(self, line_noprefix));
  if (line_nocommand.length == 0) {
    self->param_count = 0;
    self->trailing    = str_null();
    return true;
  }

  str line_nextparam = line_nocommand;
  for (usize param_idx = 0; param_idx < irc_msg_max_params; ++param_idx) {
    line_nextparam = skip_spaces(line_nextparam);
    if (line_nextparam.length == 0)  break;
    if (is_trailing(line_nextparam)) break;

    line_nextparam = parse_param(self, line_nextparam, param_idx);
    self->param_count++;
  }

  line_nextparam = skip_spaces(line_nextparam);
  if (is_trailing(line_nextparam)) {
    self->trailing = str_slice(line_nextparam, 1, line_nextparam.length);
  }
  else {
    self->trailing = str_null();
  }

  return true;
}


// MARK: - encoder
str irc_msg_encode(irc_msg *self, allocator a) {
  assert(self != NULL);

  str buffer = str_make(a, irc_msg_buffer_size);

  if (self->has_prefix) {
    if (self->prefix.is_server) {
      buffer.data[buffer.length++] = ':';
      memcpy(
        buffer.data + buffer.length,
        self->prefix.server.ident.data,
        self->prefix.server.ident.length
      );
      buffer.length += self->prefix.server.ident.length;

      buffer.data[buffer.length++] = ' ';
    }
    else {
      buffer.data[buffer.length++] = ':';
      memcpy(
        buffer.data + buffer.length,
        self->prefix.user.nick.data,
        self->prefix.user.nick.length
      );
      buffer.length += self->prefix.user.nick.length;

      buffer.data[buffer.length++] = '!';

      memcpy(
        buffer.data + buffer.length,
        self->prefix.user.ident.data,
        self->prefix.user.ident.length
      );
      buffer.length += self->prefix.user.ident.length;

      buffer.data[buffer.length++] = '@';

      memcpy(
        buffer.data + buffer.length,
        self->prefix.user.host.data,
        self->prefix.user.host.length
      );
      buffer.length += self->prefix.user.host.length;

      buffer.data[buffer.length++] = ' ';
    }
  }

  memcpy(
    buffer.data + buffer.length,
    self->command.data,
    self->command.length
  );
  buffer.length += self->command.length;

  if (self->param_count > 0) {
    for (usize i = 0; i < self->param_count; ++i) {
      buffer.data[buffer.length++] = ' ';

      memcpy(
        buffer.data + buffer.length,
        self->params[i].data,
        self->params[i].length
      );
      buffer.length += self->params[i].length;
    }
  }

  if (self->trailing.length > 0) {
    buffer.data[buffer.length++] = ' ';
    buffer.data[buffer.length++] = ':';

    memcpy(
      buffer.data + buffer.length,
      self->trailing.data,
      self->trailing.length
    );
    buffer.length += self->trailing.length;
  }

  buffer.data[buffer.length++] = '\r';
  buffer.data[buffer.length++] = '\n';

  return buffer;
}
