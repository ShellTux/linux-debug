#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
    return 1;
  }

  if (mkdir(argv[1], 0777) != 0) {
    perror("mkdir failed");
    return 1;
  }

  return 0;
}
