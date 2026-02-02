#pragma once

#include <preludec/defs.h>
#include <preludec/mem/str.h>
#include <preludec/io/stream.h>

#include "vec.h"


void write_header_prelude(stream *io);
void write_source_prelude(stream *io);

void write_prototype     (stream *io, str proc_name, VEC(sql_param) params);
void write_implementation(stream *io, str proc_name, VEC(sql_param) params, const char *input_path);
