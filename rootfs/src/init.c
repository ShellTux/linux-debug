#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define PATH "/rust/bin:/usr/local/bin:."

static void mount2(const char *mountpoint, const char *type) {
  if (mountpoint == NULL || type == NULL) {
    return;
  }

  // Create mount point directory if it doesn't exist
  if (access(mountpoint, F_OK) != 0 && mkdir(mountpoint, 0755) != 0) {
    perror("mkdir");
    _exit(1);
  }

  if (mount(type, mountpoint, type, 0, NULL) != 0) {
    fprintf(stderr, "Failed mount: %s\n", mountpoint);
  } else {
    printf("Mounted %s at %s\n", type, mountpoint);
  }
}

int main() {
  mount2("/tmp", "tmpfs");
  mount2("/proc", "proc");

  if (setenv("PATH", PATH, 1) != 0) {
    perror("setenv");
    return 1;
  }

  printf("Updated PATH: %s\n", getenv("PATH"));

  char *argv[] = {"shell", NULL};

  // Fork a child process
  pid_t pid = fork();

  if (pid < 0) {
    perror("fork failed");
    return 1;
  } else if (pid == 0) {
    // Child process: execute command
    if (execvp(argv[0], argv) == -1) {
      perror("exec failed");
    }
    exit(EXIT_FAILURE);
  } else {
    // Parent process: wait for child to finish
    wait(NULL);
  }

  _exit(69);

  return 0;
}
