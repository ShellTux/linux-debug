#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_INITIAL_CAPACITY 256

int main(int argc, char *argv[]) {
  size_t line_capacity = LINE_INITIAL_CAPACITY;
  char *line = realloc(NULL, line_capacity);
  assert(line != NULL);
  line[0] = '\0';

  for (int i = 1; i < argc; ++i) {
    const char *arg = argv[i];
    const size_t arg_len = strlen(arg) + 1; // +1 for the optional space

    const size_t line_size = strlen(line);

    if (line_size + arg_len >= line_capacity) {
      line_capacity *= 2;
      line = realloc(line, line_capacity);
      assert(line != NULL);
    }

    if (i > 1) {
      strcpy(line + line_size, " ");
      strcpy(line + line_size + 1, arg);
    } else {
      strcpy(line + line_size, arg);
    }
  }

  printf("%s\n", line);

  return 0;
}
