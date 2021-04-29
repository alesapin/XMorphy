#!/usr/bin/env python3

import sys
import random
import os

def dump_sentences(lexemes, f, total_words):
    words_counter = 0
    sentence_counter = 0
    for sentence in lexemes:
        sentence_counter += 1
        for wordform in sentence:
            words_counter += 1
            f.write(wordform + '\n')

        f.write('\n')
        if words_counter > total_words:
            break
    return sentence_counter

def mix_corpus(filename, use_val):
    sentences = []
    with open(filename, 'r') as f:
        current_sentence = []
        for line in f:
            if not line.strip():
                if current_sentence:
                    sentences.append(current_sentence)
                    current_sentence = []
                continue
            current_sentence.append(line.strip())
        if current_sentence:
            sentences.append(current_sentence)
    print("Total sentences", len(sentences))
    random.shuffle(sentences)

    total_words = sum(len(sentence) for sentence in sentences)

    if use_val:
        train = int(total_words * 0.7)
        val = int(total_words * 0.1)
        test = int(total_words * 0.2)
    else:
        train = int(total_words * 0.8)
        val = int(total_words * 0.0)
        test = int(total_words * 0.2)
    print("Train", train)
    print("Val", val)
    print("Test", test)

    fname = os.path.basename(filename)
    fname_without_extension = os.path.splitext(fname)[0]

    with open(fname_without_extension + '_group.train', 'w') as f:
        start_val = dump_sentences(sentences, f, train)

    if val != 0:
        with open(fname_without_extension + '_group.val', 'w') as f:
            start_test = dump_sentences(sentences[start_val:], f, val)
    else:
        start_test = 0

    with open(fname_without_extension + '_group.test', 'w') as f:
        test_sentences = sentences[start_test + start_val:]
        dump_sentences(test_sentences, f, test)

if __name__ == "__main__":
    mix_corpus(sys.argv[1], False)
