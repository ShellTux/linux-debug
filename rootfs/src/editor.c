#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 256

void display_help() {
  printf("Commands:\n");
  printf("  :w   - Save changes\n");
  printf("  :q   - Quit without saving\n");
  printf("  :wq  - Save and quit\n");
  printf("  :help - Show commands\n");
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s filename\n", argv[0]);
    return 1;
  }

  char *filename = argv[1];
  char *lines[MAX_LINES];
  int line_count = 0;
  int loaded = 0;   // flag to check if file is loaded
  int modified = 0; // flag to check if file was modified

  // Initialize lines to NULL
  for (int i = 0; i < MAX_LINES; i++) {
    lines[i] = NULL;
  }

  // Load file at start
  FILE *fp = fopen(filename, "r");
  if (fp != NULL) {
    char buffer[MAX_LINE_LENGTH];
    while (fgets(buffer, sizeof(buffer), fp) && line_count < MAX_LINES) {
      lines[line_count] = strdup(buffer);
      line_count++;
    }
    fclose(fp);
    loaded = 1;
    printf("Loaded file: %s\n", filename);
  } else {
    printf("File not found. Starting a new file.\n");
  }

  display_help();

  while (1) {
    printf("\n> ");
    char input[MAX_LINE_LENGTH];
    if (fgets(input, sizeof(input), stdin) == NULL) {
      break;
    }
    // Remove trailing newline
    input[strcspn(input, "\n")] = 0;

    // Check for commands starting with colon
    if (input[0] == ':') {
      if (strcmp(input, ":w") == 0) {
        // Save (write)
        FILE *fout = fopen(filename, "w");
        if (fout == NULL) {
          printf("Failed to open file for writing.\n");
        } else {
          for (int i = 0; i < line_count; i++) {
            fprintf(fout, "%s\n", lines[i]);
          }
          fclose(fout);
          modified = 0;
          printf("Saved to %s\n", filename);
        }
      } else if (strcmp(input, ":q") == 0) {
        // Quit without saving
        printf("Exiting without saving.\n");
        break;
      } else if (strcmp(input, ":wq") == 0) {
        // Save and Quit
        FILE *fout = fopen(filename, "w");
        if (fout == NULL) {
          printf("Failed to open file for writing.\n");
        } else {
          for (int i = 0; i < line_count; i++) {
            fprintf(fout, "%s\n", lines[i]);
          }
          fclose(fout);
          printf("Saved to %s\n", filename);
        }
        break;
      } else if (strcmp(input, ":help") == 0) {
        display_help();
      } else {
        printf("Unknown command. Type :help for commands.\n");
      }
    } else if (strncmp(input, "edit ", 5) == 0) {
      // Edit command: "edit <line_number>"
      int line_num = atoi(input + 5);
      if (line_num < 1 || line_num > line_count + 1) {
        printf("Invalid line number.\n");
        continue;
      }
      printf("Current content: %s",
             (line_num <= line_count) ? lines[line_num - 1] : "");
      printf("Enter new content: ");
      char new_line[MAX_LINE_LENGTH];
      if (fgets(new_line, sizeof(new_line), stdin) == NULL) {
        continue;
      }
      new_line[strcspn(new_line, "\n")] = 0;

      if (line_num == line_count + 1) {
        // Append new line
        if (line_count < MAX_LINES) {
          lines[line_count] = strdup(new_line);
          line_count++;
        } else {
          printf("Maximum lines reached.\n");
        }
      } else {
        free(lines[line_num - 1]);
        lines[line_num - 1] = strdup(new_line);
      }
      modified = 1;
    } else if (strcmp(input, "display") == 0) {
      // Display current lines
      printf("\n--- File Contents ---\n");
      for (int i = 0; i < line_count; i++) {
        printf("%d: %s\n", i + 1, lines[i]);
      }
      printf("---------------------\n");
    } else {
      // Add new line at the end
      if (line_count < MAX_LINES) {
        lines[line_count] = strdup(input);
        line_count++;
        modified = 1;
      } else {
        printf("Maximum lines reached.\n");
      }
    }
  }

  // Free allocated memory
  for (int i = 0; i < line_count; i++) {
    free(lines[i]);
  }

  return 0;
}
