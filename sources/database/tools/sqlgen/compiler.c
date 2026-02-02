#include "compiler.h"
#include "templates.h"
#include "vec.h"

#include <preludec/mem/alloc.h>


static str read_all(stream *io, allocator a) {
  iostatus status = {};

  usize contentsz = stream_size(io, &status);
  assert_release(status != IO_STATUS_ERROR);

  str content    = str_make(a, contentsz);
  content.length = stream_read(io, str_as_span(content), &status);
  assert_release(status != IO_STATUS_ERROR);

  return content;
}


static str trim_spaces(str line) {
  for (usize pos = 0; pos < line.length; ++pos) {
    if (
      line.data[pos] != '\r' &&
      line.data[pos] != '\n' &&
      line.data[pos] != '\t' &&
      line.data[pos] != ' '
    ) {
      line = str_slice(line, pos, line.length);
      break;
    }
  }

  for (usize pos = line.length; pos > 0; --pos) {
    if (
      line.data[pos - 1] != '\r' &&
      line.data[pos - 1] != '\n' &&
      line.data[pos - 1] != '\t' &&
      line.data[pos - 1] != ' '
    ) {
      line = str_slice(line, 0, pos);
      break;
    }
  }

  return line;
}


static VEC(str) split_lines(str content, allocator a) {
  VEC(str) lines = {};
  vec_str_init(&lines, a, 16);

  usize line_start = 0;
  for (usize pos = 0; pos < content.length; ++pos) {
    if (content.data[pos] == '\n') {
      str line = trim_spaces(str_slice(content, line_start, pos));
      vec_str_push(&lines, line);
      line_start = pos + 1;
    }
  }
  if (line_start < content.length) {
    str line = trim_spaces(str_slice(content, line_start, content.length));
    vec_str_push(&lines, line);
  }

  return lines;
}


static RESULT(str, UNIT) parse_special_comment(str buffer) {
  str marker = str_literal("---@");
  str prefix = str_slice(buffer, 0, marker.length);

  if (str_equal(prefix, marker)) {
    return (RESULT(str, UNIT)) OK(str_slice(buffer, marker.length, buffer.length));
  }
  else {
    return (RESULT(str, UNIT)) ERR({});
  }
}


static RESULT(str, UNIT) parse_procname(str buffer) {
  str marker = str_literal("proc ");
  str prefix = str_slice(buffer, 0, marker.length);

  if (str_equal(prefix, marker)) {
    str name = trim_spaces(str_slice(buffer, marker.length, buffer.length));
    return (RESULT(str, UNIT)) OK(name);
  }
  else {
    return (RESULT(str, UNIT)) ERR({});
  }
}


static RESULT(sql_param, UNIT) parse_param(str buffer) {
  str marker = str_literal("arg ");
  str prefix = str_slice(buffer, 0, marker.length);

  if (str_equal(prefix, marker)) {
    str remaining = trim_spaces(str_slice(buffer, marker.length, buffer.length));

    usize space_pos = 0;
    for (; space_pos < remaining.length; ++space_pos) {
      if (remaining.data[space_pos] == ' ') {
        break;
      }
    }
    if (space_pos == remaining.length) {
      return (RESULT(sql_param, UNIT)) ERR({});
    }

    str name = trim_spaces(str_slice(remaining, 0,             space_pos));
    str type = trim_spaces(str_slice(remaining, space_pos + 1, remaining.length));

    sql_param param = { name, type };
    return (RESULT(sql_param, UNIT)) OK(param);
  }
  else {
    return (RESULT(sql_param, UNIT)) ERR({});
  }
}


void compile_input(
  const char *input_path,

  stream *input,
  stream *output_header,
  stream *output_source
) {
  assert(input_path    != NULL);
  assert(input         != NULL);
  assert(output_header != NULL);
  assert(output_source != NULL);

  allocator a       = std_allocator();
  str       content = read_all(input, a);
  VEC(str)  lines   = split_lines(content, a);

  str            proc_name   = str_null();
  VEC(sql_param) params      = {};
  vec_sql_param_init(&params, a, 4);

  for (usize line_idx = 0; line_idx < lines.count; ++line_idx) {
    str line = lines.data[line_idx];

    auto directive_res = parse_special_comment(line);
    if (directive_res.is_ok) {
      str directive = directive_res.ok;

      auto proc_res = parse_procname(directive);
      if (proc_res.is_ok) {
        proc_name = proc_res.ok;
        continue;
      }

      auto param_res = parse_param(directive);
      if (param_res.is_ok) {
        vec_sql_param_push(&params, param_res.ok);
        continue;
      }
    }
  }

  write_prototype     (output_header, proc_name, params);
  write_implementation(output_source, proc_name, params, input_path);

  vec_sql_param_deinit(&params);
  vec_str_deinit(&lines);
  str_free(a, &content);
}
