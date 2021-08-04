#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_LETTERS 26
#define PUZZLE_CHAR_COUNT 7
#define MIN_WORD_LEN 4
// '0b00000011111111111111111111111111' (26 '1's)
#define PUZZLE_LETTERS_MASK (INT32_MAX ^ ((((1 << 5) - 1)) << NUM_LETTERS))
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
 * Like the word signature mentioned above.
 * A `int32_t` is chosen to represent the signature of the puzzle.
 *
 * First 6 bits are used to store offsets of the center letter from 'A'.
 */
int32_t puzzle_signature(const char *puzzle) {
  return word_signature(puzzle) | (char_index(puzzle[0]) << NUM_LETTERS);
}

/*
 * Checks whether the signature of a word matches the signature of the puzzle
 */
bool check_signature(const int32_t signature, const int32_t puzzle) {
  // Left shift 26 times to zero out the puzzle signature first and get the
  // center letter index Then do left shift to generate center letter signature
  int32_t center_letter_signature = 1 << (puzzle >> NUM_LETTERS);

  // Must contain center letter
  if ((center_letter_signature & signature) != center_letter_signature) {
    return false;
  }

  // Check for no extra letters (bits)
  return (signature | puzzle) == puzzle;
}

/*
 * Checks wheter a word is a solution.
 *
 * It must:
 * Contain at least 4 letters (letters can be reused)
 * Include the center letter
 */
enum check_result check_word(const char *word, const int32_t puzzle) {
  size_t char_counts = 0;
  for (size_t i = 0; i < strnlen(word, MAX_LEN); i++) {
    // Only count letters
    if (isalpha(word[i])) {
      char_counts++;
    }
  }

  int32_t signature_for_word = word_signature(word);
  if (char_counts < MIN_WORD_LEN) {
    return NOT_MATCH; // Word too short
  } else if (signature_for_word == (puzzle & PUZZLE_LETTERS_MASK)) {
    return PANGRAM;
  } else if (check_signature(word_signature(word), puzzle)) {
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

int main(int argc, char const *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <puzzle_letters_starting_with_center_letter>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if (!check_arg(argv[1])) {
    printf("The puzzle letters should be of size 7 and no duplicates\n");
    exit(EXIT_FAILURE);
  }

  int32_t puzzle = puzzle_signature(argv[1]);

  char line[MAX_LEN];
  FILE *file = fopen("/usr/share/dict/words", "r");
  if (file == NULL) {
    exit(EXIT_FAILURE);
  }

  while (fgets(line, sizeof(line), file)) {
    switch (check_word(line, puzzle)) {
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
