#include <dirent.h>
#include <stdio.h>
#include <string.h>

int main() {
  const char *sys_block_path = "/sys/block";
  DIR *dir = opendir(sys_block_path);
  if (dir == NULL) {
    perror("opendir");
    return 1;
  }

  struct dirent *entry;
  printf("NAME\n");
  while ((entry = readdir(dir)) != NULL) {
    // Skip "." and ".."
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;
    printf("%s\n", entry->d_name);
  }

  closedir(dir);
  return 0;
}
