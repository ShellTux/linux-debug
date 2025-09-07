#include <limits.h> // for PATH_MAX
#include <stdio.h>
#include <unistd.h> // for getcwd

int main() {
  char cwd[PATH_MAX] = {0};

  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("%s\n", cwd);
  } else {
    perror("getcwd() error");
    return 1;
  }

  return 0;
}
