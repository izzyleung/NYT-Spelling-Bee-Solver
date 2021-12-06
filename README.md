# New York Times Spelling Bee Solver
Reads words from `/usr/share/dict/words` and find all matching words for the Spelling Bee puzzle from the New York Times

# Build
`make main`

# Usage
```sh
$ # ./solver [-f words_file_path] <puzzle_letters_starting_with_center_letter>
$ ./solver FMYINAR
AFAR
...
INFIRM
* INFIRMARY
MAFIA
...
RIFFRAFF
```

# Requirements
- Some *nix system that has `/usr/share/dict/words`
- Clang/GCC
- Clang-Format (optional, install with `brew install clang-format` on macOS)
- A curated list of words accepted by the Spelling Bee (optional)

# Explanation
A `int32_t` is chosen to represent the signature of a word.
Where the last 26 bits are used to store wheter a letter exists in it.

LSB is used to represent whether 'A' exists in the word.
7th bit from left, or 26th bit from right, represents whether 'Z' exists in the word.

For the signature of a puzzle, the above signature is adopted with one change.
The first 6 bits are used to represent the offest of the center letter from 'A'.

For example, the signature for puzzle `*F*MYINAR` is `352465185`.
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
$ ./solver 'ncfiotu' | python3 stats.py
WORDS: 42, POINTS: 205, PANGRAMS: 1

     4   5   6   7   8   9  10   Σ
C:   1   2   4   2   -   -   1   10
F:   1   2   1   1   1   -   -   6
I:   3   1   2   -   -   1   -   7
N:   2   -   1   -   -   -   1   4
O:   1   1   -   -   -   -   -   2
T:   2   3   -   1   -   -   -   6
U:   2   3   1   1   -   -   -   7
Σ:  12  12   9   5   1   1   2   42
```
