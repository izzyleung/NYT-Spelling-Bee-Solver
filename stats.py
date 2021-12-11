#!/usr/bin/env python3

import sys

from collections import defaultdict


def main():
    words = [w.strip() for w in sys.stdin.readlines()]

    initial_counts, length_counts, two_letters = defaultdict(dict), {}, {}
    points, max_word_len, num_pangrams, num_perfects = 0, 0, 0, 0

    for word in words:
        # It's a pangram
        if word.startswith('* '):
            # Strip the first 2 chars
            word = word[2:]
            # Add the bouns 7 points to the points total
            points += 7
            # Increase the total amount of pangrams
            num_pangrams += 1
            # Perfect Pangram — A pangram that uses each letter only once
            if len(word) == 7:
                num_perfects += 1

        # Update points total
        if len(word) == 4:
            # 4 letter words are worth 1 point each
            points += 1
        else:
            # Longer words earn 1 point per letter
            points += len(word)
            # Update `max_word_len` for outputting results later
            max_word_len = max(max_word_len, len(word))

        word_initial, word_len, first_two = word[0], len(word), word[:2]
        initial_info = initial_counts[word_initial]
        initial_info[word_len] = initial_info.get(word_len, 0) + 1
        length_counts[word_len] = length_counts.get(word_len, 0) + 1
        two_letters[first_two] = two_letters.get(first_two, 0) + 1

    puzzle_stats = [
        f'WORDS: {len(words)}',
        f'POINTS: {points}',
    ]
    
    pangram_stat = f'PANGRAMS: {num_pangrams}'
    if num_perfects > 0:
        pangram_stat += f' ({num_perfects} Perfect)'
    puzzle_stats.append(pangram_stat)
    # BINGO — All seven letters in the puzzle are used to start at least one word in the word list.
    if len(initial_counts) == 7:
        puzzle_stats.appeng('BINGO')

    # Print the stats
    print(', '.join(puzzle_stats), '\n')

    word_lengths = list(sorted(length_counts.keys()))
    # Print the table header
    print(f"   {' '.join(str(i).rjust(3) for i in word_lengths)}   Σ")

    word_initials = sorted(initial_counts.keys())
    # Print the info for each letter
    for c in word_initials:
        counts = []
        for i in word_lengths:
            counts.append(initial_counts[c].get(i, 0))
        total = sum(counts)

        cells = [(str(i) if i != 0 else '-').rjust(3) for i in counts]
        print(f'{c}: {" ".join(cells)}   {str(total)}')
    # Print the info on each word size
    counts = []
    for i in word_lengths:
        count = 0
        for c in initial_counts.keys():
            count += initial_counts[c].get(i, 0)
        counts.append(count)
    total = sum(counts)

    cells = [(str(i) if i != 0 else '-').rjust(3) for i in counts]
    print(f'Σ: {" ".join(cells)}   {str(total)}', '\n')

    # Print two letter list
    two_letters_stats = defaultdict(list)
    for first_two, count in two_letters.items():
        word_initial = first_two[0]
        two_letters_stats[word_initial].append(f'{first_two}-{count}')

    print('Two letter list:')
    for word_initial in word_initials:
        print(' '.join(two_letters_stats[word_initial]))


if __name__ == '__main__':
    main()
