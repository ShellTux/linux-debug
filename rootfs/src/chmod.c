// chmod.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <mode> <file>\n", argv[0]);
    return 1;
  }

  int mode = strtol(argv[1], NULL, 8); // mode in octal
  if (chmod(argv[2], mode) != 0) {
    perror("chmod failed");
    return 1;
  }

  return 0;
}
