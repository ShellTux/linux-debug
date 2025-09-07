#include <stdio.h>
#include <sys/statvfs.h>

int main() {
  const char *path = "."; // current directory
  struct statvfs buf;

  if (statvfs(path, &buf) != 0) {
    perror("statvfs");
    return 1;
  }

  const unsigned long long total = buf.f_blocks * buf.f_frsize;
  const unsigned long long available = buf.f_bavail * buf.f_frsize;
  const unsigned long long used = total - available;

  printf("Filesystem at %s:\n", path);
  printf("Total: %llu bytes\n", total);
  printf("Used:  %llu bytes\n", used);
  printf("Available: %llu bytes\n", available);

  return 0;
}
