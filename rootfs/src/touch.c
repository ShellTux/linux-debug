#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <file>\n", argv[0]);
    return 1;
  }

  // Open file for writing, create if it doesn't exist
  int fd = open(argv[1], O_WRONLY | O_CREAT, 0666);
  if (fd < 0) {
    perror("open failed");
    return 1;
  }
  close(fd);

  // Update timestamp
  struct utimbuf new_times;
  new_times.actime = time(NULL);  // access time
  new_times.modtime = time(NULL); // modification time
  if (utime(argv[1], &new_times) != 0) {
    perror("utime failed");
    return 1;
  }
  return 0;
}
