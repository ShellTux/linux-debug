#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <filepath>\n", argv[0]);
    return 1;
  }

  const char *filepath = argv[1];
  FILE *file = fopen(filepath, "rb"); // Open in binary mode for accuracy
  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  // Seek to the end to determine file size
  fseek(file, 0, SEEK_END);
  const long filesize = ftell(file);
  rewind(file); // Go back to the beginning

  // Allocate memory for the entire file content
  char *buffer = malloc(filesize + 1); // +1 for null terminator
  if (buffer == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    fclose(file);
    return 1;
  }

  // Read the entire file into buffer
  const size_t read_size = fread(buffer, 1, filesize, file);
  if (read_size != (size_t)filesize) {
    fprintf(stderr, "Error reading file\n");
    free(buffer);
    fclose(file);
    return 1;
  }

  buffer[filesize] = '\0'; // Null-terminate the string

  // Print the content
  printf("%s", buffer);

  // Cleanup
  free(buffer);
  fclose(file);

  return 0;
}
