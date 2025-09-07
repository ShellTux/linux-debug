#include <dirent.h>
#include <getopt.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

typedef struct {
  bool enable_color;
  bool long_list;
  bool show_all;
  bool show_hidden;
  bool show_help;

  char *dir;
} Cli;

#define BLUE(message) "\033[34m" message "\033[0m"
#define GREEN(message) "\033[32m" message "\033[0m"

void usage(const char *prog_name) {
  printf("Usage: %s [OPTION]... DIR\n", prog_name);
  printf("List files.\n\n");
  printf("Options:\n");
  printf("  -a           Show all\n");
  printf("  -A           Show hidden\n");
  printf("  -c, --color  Enable color\n");
  printf("  -h, --help  Display this help and exit\n");
  printf("  -l           Long list\n");
}

typedef enum {
  Regular,
  Directory,
  Executable,
} TypeFile;

int main(int argc, char *argv[]) {
  int opt = 0;
  Cli cli = {0};

  // Define long options
  static struct option long_options[] = {{"color", no_argument, 0, 'c'},
                                         {"help", no_argument, 0, 'h'},
                                         {0, 0, 0, 0}};

  // Parse options
  while ((opt = getopt_long(argc, argv, "aAchl", long_options, NULL)) != -1) {
    switch (opt) {
    case 'a':
      cli.show_all = true;
      break;
    case 'A':
      cli.show_hidden = true;
      break;
    case 'c':
      cli.enable_color = true;
      break;
    case 'h':
      cli.show_help = true;
      break;
    case 'l':
      cli.long_list = true;
      break;
    default:
      usage(argv[0]);
      return 1;
    }
  }

  if (cli.show_help) {
    usage(argv[0]);
    return 0;
  }

  cli.dir = argv[optind] == NULL ? "." : argv[optind];

  DIR *dir = opendir(cli.dir);
  if (dir == NULL) {
    perror("opendir");
    return 1;
  }

  for (struct dirent *entry = readdir(dir); entry != NULL;
       entry = readdir(dir)) {
    // Skip hidden files unless -a or -A is specified
    if (!cli.show_hidden && !cli.show_all && entry->d_name[0] == '.') {
      continue;
    }

    if (cli.show_all &&
        (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)) {
      continue;
    }

    char path[1024];
    snprintf(path, sizeof(path), "%s/%s", cli.dir, entry->d_name);
    struct stat st;
    if (stat(path, &st) == -1) {
      perror("stat");
      continue;
    }

    const TypeFile typeFile = S_ISDIR(st.st_mode) ? Directory
                              : st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)
                                  ? Executable
                                  : Regular;

    if (cli.long_list) {
      // File permissions
      char perms[11] = "----------";
      if (S_ISDIR(st.st_mode))
        perms[0] = 'd';
      if (st.st_mode & S_IRUSR)
        perms[1] = 'r';
      if (st.st_mode & S_IWUSR)
        perms[2] = 'w';
      if (st.st_mode & S_IXUSR)
        perms[3] = 'x';
      if (st.st_mode & S_IRGRP)
        perms[4] = 'r';
      if (st.st_mode & S_IWGRP)
        perms[5] = 'w';
      if (st.st_mode & S_IXGRP)
        perms[6] = 'x';
      if (st.st_mode & S_IROTH)
        perms[7] = 'r';
      if (st.st_mode & S_IWOTH)
        perms[8] = 'w';
      if (st.st_mode & S_IXOTH)
        perms[9] = 'x';

      const nlink_t nlinks = st.st_nlink; // Number of links
      const off_t size = st.st_size;      // Size

      char timebuf[64]; // Time
      struct tm *tm_info = localtime(&st.st_mtime);
      strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm_info);

      // Print long listing
      if (cli.enable_color) {
        switch (typeFile) {
        case Regular:
          printf("%s %lu %5lld %s %s\n", perms, (unsigned long)nlinks,
                 (long long)size, timebuf, entry->d_name);
          break;
        case Directory:
          printf("%s %lu %5lld %s " BLUE("%s") "\n", perms,
                 (unsigned long)nlinks, (long long)size, timebuf,
                 entry->d_name);
          break;
        case Executable:
          printf("%s %lu %5lld %s " GREEN("%s") "\n", perms,
                 (unsigned long)nlinks, (long long)size, timebuf,
                 entry->d_name);
          break;
        }
      } else {
        printf("%s %lu %5lld %s %s\n", perms, (unsigned long)nlinks,
               (long long)size, timebuf, entry->d_name);
      }
    } else {
      if (cli.enable_color) {
        switch (typeFile) {
        case Regular:
          printf("%s\n", entry->d_name);
          break;
        case Directory:
          printf(BLUE("%s") "\n", entry->d_name);
          break;
        case Executable:
          printf(GREEN("%s") "\n", entry->d_name);
          break;
        }
      } else {
        printf("%s\n", entry->d_name);
      }
    }
  }

  closedir(dir);
  return 0;
}
