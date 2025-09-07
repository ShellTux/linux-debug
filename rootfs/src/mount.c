#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>

int list_mounts() {
  FILE *fp = fopen("/proc/mounts", "r");
  if (!fp) {
    perror("fopen");
    return -1;
  }
  char line[1024];
  printf("Currently mounted filesystems:\n");
  while (fgets(line, sizeof(line), fp)) {
    printf("%s", line);
  }
  fclose(fp);
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    // No arguments: list mounts
    return list_mounts();
  } else {
    // Arguments provided: mount filesystem
    if (argc != 4) {
      fprintf(stderr, "Usage: %s <source> <target> <fs_type>\n", argv[0]);
      return 1;
    }

    const char *source = argv[1];
    const char *target = argv[2];
    const char *type = argv[3];

    // Mount options can be NULL or customized
    if (mount(source, target, type, 0, NULL) != 0) {
      perror("mount");
      return 1;
    } else {
      printf("Mounted %s at %s as %s\n", source, target, type);
    }
    return 0;
  }
}
