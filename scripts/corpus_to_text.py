#!/usr/bin/env python3

import sys

if __name__ == "__main__":
    with open(sys.argv[1], 'r') as corpus:
        sentence = []
        for line in corpus:
            if not line.strip():
                print(' '.join(sentence))
                sentence = []
            else:
                sentence.append(line.split('\t')[1])
        if sentence:
            print(' '.join(sentence))
