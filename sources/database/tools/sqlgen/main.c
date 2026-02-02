#include <preludec/defs.h>
#include <preludec/mem/str.h>
#include <preludec/io/stream.h>

#include "templates.h"
#include "compiler.h"


int main(int argc, char *argv[static argc]) {
  if (argc < 4) {
    fprintf(stderr, "Usage: %s <output.h> <output.c> <input.sql...>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char  *output_header_path = argv[1];
  const char  *output_source_path = argv[2];
  const char **input_sql_paths    = (const char **)&argv[3];
  usize        input_sql_count    = (usize)(argc - 3);

  FILE *output_header_file = fopen(output_header_path, "w");
  if (output_header_file == NULL) {
    fprintf(stderr, "ERROR: Failed to open '%s' (write): %s\n", output_header_path, strerror(errno));
    return EXIT_FAILURE;
  }

  FILE *output_source_file = fopen(output_source_path, "w");
  if (output_source_file == NULL) {
    fprintf(stderr, "ERROR: Failed to open '%s' (write): %s\n", output_source_path, strerror(errno));
    fclose(output_header_file);
    return EXIT_FAILURE;
  }

  stream output_header_stream = make_filestream(output_header_file);
  stream output_source_stream = make_filestream(output_source_file);

  write_header_prelude(&output_header_stream);
  write_source_prelude(&output_source_stream);

  for (usize input_idx = 0; input_idx < input_sql_count; ++input_idx) {
    FILE *input_sql_file = fopen(input_sql_paths[input_idx], "r");
    if (input_sql_file == NULL) {
      fprintf(stderr, "ERROR: Failed to open '%s' (read): %s\n", input_sql_paths[input_idx], strerror(errno));
      assert_release(true == stream_close(&output_header_stream, NULL));
      assert_release(true == stream_close(&output_source_stream, NULL));
      return EXIT_FAILURE;
    }

    stream input_sql_stream = make_filestream(input_sql_file);
    compile_input(
      input_sql_paths[input_idx],
      &input_sql_stream,
      &output_header_stream,
      &output_source_stream
    );
    assert_release(true == stream_close(&input_sql_stream, NULL));
  }

  assert_release(true == stream_close(&output_header_stream, NULL));
  assert_release(true == stream_close(&output_source_stream, NULL));

  return EXIT_SUCCESS;
}
