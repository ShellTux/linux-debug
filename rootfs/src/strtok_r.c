#include <stdio.h>

#define MAX_WORDS 64

static inline char *strtok_r(char *text, const char *delimitors,
                             char **save_this) {
  if (save_this == NULL) {
    return NULL;
  }

  if (text != NULL) {
    /* New text. */
    for (int i = 0; text[i] != '\0'; ++i) {
      for (char *d = (char *)delimitors; *d != '\0'; d++) {
        if (text[i] == *d) {
          text[i] = '\0';
          *save_this = &text[i + 1];
          return text;
        }
      }
    }
  } else if ((save_this != NULL) && (*save_this != NULL)) {
    /* Old text. */
    char *start = *save_this;
    for (int i = 0; (*save_this)[i] != '\0'; ++i) {
      for (char *d = (char *)delimitors; *d != '\0'; ++d) {
        if ((*save_this)[i] == *d) {
          (*save_this)[i] = '\0';
          *save_this = &((*save_this)[i + 1]);
          return start;
        }
      }
    }
    *save_this = NULL;
    save_this = NULL;
    return start;
  }
  return NULL;
}

static inline void reverseArray(char **arr, size_t size) {
  size_t start = 0;
  size_t end = size - 1;
  while (start < end) {
    // Swap elements at start and end
    char *temp = arr[start];
    arr[start] = arr[end];
    arr[end] = temp;
    start++;
    end--;
  }
}

int main(void) {
  char *saveptr = NULL;

  char string[] = "foo bar baz";
  printf("%s\n", string);

  char *words[MAX_WORDS] = {0};
  size_t words_size = 0;

  for (char *word = strtok_r(string, " ", &saveptr); word != NULL;
       word = strtok_r(NULL, " ", &saveptr)) {
    words[words_size++] = word;
  }

  for (size_t i = 0; i < words_size; ++i) {
    printf("%zu: %s\n", i, words[i]);
  }

  printf("\nReverse Array\n");

  reverseArray(words, words_size);

  for (size_t i = 0; i < words_size; ++i) {
    printf("%zu: %s\n", i, words[i]);
  }

  return 0;
}
