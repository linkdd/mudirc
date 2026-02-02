#pragma once

#include <preludec/defs.h>
#include <preludec/io/stream.h>


void compile_input(
  const char *input_path,

  stream *input,
  stream *output_header,
  stream *output_source
);
