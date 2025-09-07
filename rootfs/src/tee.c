#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  bool quiet;
  bool append;
  bool show_help;
  char *filename;
} Cli;

void print_help() {
  printf("Usage: tee [OPTION]... FILE\n");
  printf("Copy standard input to FILE, and also to standard output.\n\n");
  printf("Options:\n");
  printf("  -q, --quiet  Don't write to stdout\n");
  printf("  -a           Append to the FILE instead of overwriting\n");
  printf("  -h, --help  Display this help and exit\n");
}

int main(int argc, char *argv[]) {
  int opt;

  Cli cli = {0};

  // Define long options
  static struct option long_options[] = {{"quiet", no_argument, 0, 'q'},
                                         {"help", no_argument, 0, 'h'},
                                         {0, 0, 0, 0}};

  // Parse options
  while ((opt = getopt_long(argc, argv, "aqh", long_options, NULL)) != -1) {
    switch (opt) {
    case 'a':
      cli.append = true;
      break;
    case 'h':
      cli.show_help = true;
      break;
    case 'q':
      cli.quiet = true;
      break;
    default:
      print_help();
      return 1;
    }
  }

  if (cli.show_help) {
    print_help();
    return 0;
  }

  // Remaining argument should be filename
  if (optind >= argc) {
    fprintf(stderr, "Error: No output file specified.\n");
    print_help();
    return 1;
  }
  cli.filename = argv[optind];

  // Open the output file
  FILE *file = fopen(cli.filename, cli.append ? "a" : "w");
  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  // Read from stdin and write to stdout and file
  char buffer[1024];
  while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    if (!cli.quiet) {
      fputs(buffer, stdout);
    }

    fputs(buffer, file);
  }

  fclose(file);
  return 0;
}
