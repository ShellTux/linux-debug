#include <assert.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
  printf("argc = %d\n", argc);
  printf("argv = %p\n", argv);

  for (int i = 0; i < argc; ++i) {
    printf("argv[%d] = %s\n", i, argv[i]);
  }

  _exit(4);

  return 0;
}
