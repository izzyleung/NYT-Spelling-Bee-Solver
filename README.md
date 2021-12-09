# New York Times Spelling Bee Solver
Reads words from `/usr/share/dict/words` or your simple one word per line file, and find all matching words for the Spelling Bee puzzle from the New York Times.

With a complimentary Python program that chews on the output and spews out stats in the format from the Spelling Bee Forum.

# Build
`make main`

# Usage
```sh
$ # ./solver [-f words_file_path] <puzzle_letters_starting_with_center_letter>
$ ./solver FAIMNRY
AFAR
...
INFIRM
* INFIRMARY
MAFIA
...
RIFFRAFF
```

# Requirements
- Clang/GCC
- Some *nix-ish system that has `/usr/share/dict/words` (optional)
- Clang-Format (optional, install with `brew install clang-format` on macOS)
- A curated list of words accepted by the Spelling Bee (optional)

# Explanation
A `int32_t` is chosen to represent the signature of a word.
Where the last 26 bits are used to store wheter a letter exists in it.

LSB is used to represent whether 'A' exists in the word.
7th bit from left, or 26th bit from right, represents whether 'Z' exists in the word.

For the signature of a puzzle, the above signature is adopted with one change.
The first 6 bits are used to represent the offest of the center letter from 'A'.

For example, the signature for puzzle `*F*AIMNRY` is `352465185`.
And a short explanation can be found below.
```
bin(352465185) == '0b00010101000000100011000100100001'

000101 0 1 0 0 0 0 0 0 1 0 0 0 1 1 0 0 0 1 0 0 1 0 0 0 0 1
       ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^
       Z Y X W V U T S R Q P O N M L K J I H G F E D C B A
The right most 26 bits represent all letters contained in the puzzle.

0b000101 == 5
Which is the offset of 'F' from 'A'.
```

# Output stats
```
$ ./solver -f words 'ibgorty' | python3 stats.py
WORDS: 21, POINTS: 68, PANGRAMS: 1 (1 Perfect), BINGO

     4   5   6   7   Σ
B:   2   1   -   1   4
G:   2   1   1   -   4
I:   1   -   -   -   1
O:   1   1   -   -   2
R:   2   2   1   -   5
T:   3   1   -   -   4
Y:   1   -   -   -   1
Σ:  12   6   2   1   21
```
