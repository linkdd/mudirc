#include <preludec/defs.h>


int main(int argc, char *argv[static argc]) {
  if (argc < 4) {
    fprintf(stderr, "Usage: %s <output.h> <output.c> <input.sql...>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char  *output_header_path = argv[1];
  const char  *output_source_path = argv[2];
  const char **input_sql_paths    = (const char **)&argv[3];
  usize        input_sql_count    = (usize)(argc - 3);

  printf("Write header to: %s\n", output_header_path);
  printf("Write source to: %s\n", output_source_path);

  for (usize i = 0; i < input_sql_count; i++) {
    printf("Read input SQL: %s\n", input_sql_paths[i]);
  }

  return EXIT_FAILURE;
}
