#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

void disable_raw_mode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); }

void enable_raw_mode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disable_raw_mode);

  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
  raw.c_cc[VMIN] = 1;
  raw.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// Helper to refresh the line display
void refresh_line(const char *line, int cursor_pos, int line_len) {
  // Clear line
  printf("\r\033[K");
  printf("> %s", line);
  // Move cursor to correct position
  int prompt_len = 2; // '> '
  printf("\r\033[%dC", prompt_len + cursor_pos);
  fflush(stdout);
}

int main() {
  enable_raw_mode();

  char line[1024] = {0};
  int line_len = 0;
  int cursor_pos = 0;

  printf("Simple shell. Press Ctrl+D to exit.\n");
  printf("> ");
  fflush(stdout);

  while (1) {
    char c;
    ssize_t nread = read(STDIN_FILENO, &c, 1);
    if (nread <= 0)
      break; // EOF or error

    if (c == 4) { // Ctrl+D
      break;
    } else if (c == 0x01) { // Ctrl+A
      // Move cursor to start
      cursor_pos = 0;
    } else if (c == 0x05) { // Ctrl+E
      // Move cursor to end
      cursor_pos = line_len;
    } else if (c == 0x10) { // Ctrl+P
      // Placeholder for previous command
      printf("\n[Ctrl+P] pressed (not implemented)\n");
      printf("> %s", line);
      fflush(stdout);
    } else if (c == 0x0e) { // Ctrl+N
      // Placeholder for next command
      printf("\n[Ctrl+N] pressed (not implemented)\n");
      printf("> %s", line);
      fflush(stdout);
    } else if (c == 0x1b) {
      // Possible escape sequence (like Alt+F / Alt+B)
      char seq[3];
      if (read(STDIN_FILENO, &seq[0], 1) != 1)
        continue;
      if (read(STDIN_FILENO, &seq[1], 1) != 1)
        continue;

      if (seq[0] == '[') {
        if (seq[1] == 'C') {
          // Right arrow (simulate Alt+F)
          if (cursor_pos < line_len)
            cursor_pos++;
        } else if (seq[1] == 'D') {
          // Left arrow (simulate Alt+B)
          if (cursor_pos > 0)
            cursor_pos--;
        }
      }
    } else if (c == 127 || c == 8) {
      // Backspace
      if (cursor_pos > 0) {
        memmove(&line[cursor_pos - 1], &line[cursor_pos],
                line_len - cursor_pos + 1);
        line_len--;
        cursor_pos--;
      }
    } else if (c == '\r' || c == '\n') {
      // Enter key
      printf("\n");
      // Here, normally you'd process the command
      // For now, just clear the line
      line[0] = '\0';
      line_len = 0;
      cursor_pos = 0;
      printf("> ");
    } else {
      // Insert character at cursor position
      if (line_len < sizeof(line) - 1) {
        memmove(&line[cursor_pos + 1], &line[cursor_pos],
                line_len - cursor_pos + 1);
        line[cursor_pos] = c;
        line_len++;
        cursor_pos++;
      }
    }

    refresh_line(line, cursor_pos, line_len);
  }

  disable_raw_mode();
  printf("\nExiting...\n");
  return 0;
}
