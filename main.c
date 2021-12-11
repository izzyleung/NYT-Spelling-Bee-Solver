#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_LETTERS 26
#define PUZZLE_CHAR_COUNT 7
#define MIN_WORD_LEN 4
// '0b00000011111111111111111111111111' (26 '1's)
#define PUZZLE_LETTERS_MASK (1 << NUM_LETTERS) - 1
#define MAX_LEN 128

enum check_result { NOT_MATCH, MATCH, PANGRAM };

/*
 * Offsets from the upper cased letter to 'A'
 */
short char_index(const char letter) { return toupper(letter) - 'A'; }

/*
 * A `int32_t` is chosen to represent the signature of a word.
 *
 * A signature being: the existence of letters from 'A' to 'Z'.
 *
 * Starting from the LSB as wheter letter 'A' exists
 * All the way to the 26th bit from right representing whether letter 'Z' exists
 */
int32_t word_signature(const char *word) {
  int32_t result = 0;

  for (size_t i = 0; i < strnlen(word, MAX_LEN); i++) {
    char letter = word[i];
    if (!isalpha(letter)) {
      continue;
    }

    result |= (1 << char_index(letter));
  }

  return result;
}

/*
 * Checks wheter a word is a solution.
 *
 * It must:
 * Contain at least 4 letters (letters can be reused)
 * Include the center letter
 */
enum check_result check_word(const char *word, const int32_t puzzle,
                             const int32_t center_letter) {
  size_t char_counts = 0;
  for (size_t i = 0; i < strnlen(word, MAX_LEN); i++) {
    // Only count letters
    if (isalpha(word[i])) {
      char_counts++;
    }
  }

  if (char_counts < MIN_WORD_LEN) {
    return NOT_MATCH; // Word too short
  }

  int32_t signature_for_word = word_signature(word);
  if ((signature_for_word & center_letter) != center_letter) {
    return NOT_MATCH; // Does not contain center letter
  } else if (signature_for_word == puzzle) {
    return PANGRAM;
  } else if ((signature_for_word | puzzle) == puzzle) {
    return MATCH;
  } else {
    return NOT_MATCH;
  }
}

/*
 * Checks whether the command line arg contains 7 unique letters.
 */
bool check_arg(const char *arg) {
  if (strnlen(arg, MAX_LEN) != PUZZLE_CHAR_COUNT) {
    return false;
  }

  int32_t signature = 0;
  for (size_t i = 0; i < PUZZLE_CHAR_COUNT; i++) {
    char letter = arg[i];
    if (!isalpha(letter)) {
      return false;
    }

    int32_t target = 1 << char_index(letter);

    // Bit already set to 1, duplicate!
    if ((signature & target) == target) {
      return false;
    }

    signature |= target;
  }

  return true;
}

int main(int argc, char *const *argv) {
  int option;
  char *words_file_path = "/usr/share/dict/words";

  while ((option = getopt(argc, argv, "f:")) != -1) {
    switch (option) {
    case 'f':
      words_file_path = optarg;
      break;
    default:
      break;
    }
  }

  if (optind != argc - 1) {
    printf("Usage: %s [-f <words_list_path>] "
           "<puzzle_letters_starting_with_center_letter>\n",
           argv[0]);
    exit(EXIT_FAILURE);
  }

  if (!check_arg(argv[optind])) {
    printf("The puzzle letters should be of size 7 and no duplicates\n");
    exit(EXIT_FAILURE);
  }
  char *puzzle_letters = argv[optind];
  int32_t puzzle = word_signature(puzzle_letters);
  int32_t center_letter = 1 << char_index(puzzle_letters[0]);

  char line[MAX_LEN];
  FILE *file = fopen(words_file_path, "r");
  if (file == NULL) {
    printf("Cannot open file at path: %s\n", words_file_path);
    exit(EXIT_FAILURE);
  }

  while (fgets(line, sizeof(line), file)) {
    switch (check_word(line, puzzle, center_letter)) {
    case PANGRAM:
      printf("* ");
    case MATCH:
      for (size_t i = 0; i < strnlen(line, MAX_LEN); i++) {
        if (!isalpha(line[i])) {
          continue;
        }
        printf("%c", toupper(line[i]));
      }
      printf("\n");
      break;
    default:
      continue;
    }
  }

  fclose(file);
  exit(EXIT_SUCCESS);
}
