import sys
import random
import os

MAXLEN = 20

def dump_lexemes(lexemes, f, total_words, only_lemmas):
    words_counter = 0
    lexeme_counter = 0
    for lexeme in lexemes:
        lexeme_counter += 1
        if only_lemmas:
            words_counter += 1
            f.write(lexeme[0] + '\n')
        else:
            for wordform in lexeme:
                words_counter += 1
                f.write(wordform + '\n')
        if words_counter > total_words:
            break
    return lexeme_counter

def dump_words(words, f, start, count):
    for word in words[start:start + count]:
        f.write(word + '\n')

def mix_lexemes(filename, use_lemmas):
    lexemes = []
    with open(filename, 'r') as f:
        current_lexeme = []
        for line in f:
            if not line.strip():
                if current_lexeme:
                    lexemes.append(current_lexeme)
                    current_lexeme = []
                continue
            word = line.split('\t')[0]
            if len(word) > MAXLEN:
                continue
            current_lexeme.append(line.strip())
        if current_lexeme:
            lexemes.append(current_lexeme)
    print("Total lexemes", len(lexemes))
    random.shuffle(lexemes)

    if use_lemmas:
        total_words = len(lexemes)
    else:
        total_words = sum(len(lexeme) for lexeme in lexemes)

    train = int(total_words * 0.7)
    val = int(total_words * 0.1)
    test = int(total_words * 0.2)
    print("Train", train)
    print("Val", val)
    print("Test", test)
    fname = os.path.basename(filename)
    if use_lemmas:
        interpart = '_lemma_'
    else:
        interpart = '_lexeme_'

    fname_without_extension = os.path.splitext(fname)[0]
    with open(fname_without_extension + interpart + 'group.train', 'w') as f:
        start_val = dump_lexemes(lexemes, f, train, use_lemmas)

    with open(fname_without_extension + interpart + 'group.val', 'w') as f:
        start_test = dump_lexemes(lexemes[start_val:], f, val, use_lemmas)

    with open(fname_without_extension + interpart + 'group.test', 'w') as f:
        test_lexemes = lexemes[start_test + start_val:]
        dump_lexemes(test_lexemes, f, test, use_lemmas)

    if use_lemmas:
        with open(fname_without_extension + '_lexeme_group.test', 'w') as f:
            dump_lexemes(test_lexemes, f, 100000000, False)
    else:
        with open(fname_without_extension + '_lemma_group.test', 'w') as f:
            test_lemmas = [[l[0]] for l in test_lexemes]
            dump_lexemes(test_lemmas, f, 100000000, True)


def mix_words(filename):
    words = []
    lemmas = set([])
    with open(filename, 'r') as f:
        is_lemma = True
        for line in f:
            if line.strip():
                word = line.split('\t')
                if len(word) > MAXLEN:
                    is_lemma = False
                    continue
                words.append(line.strip())
                if is_lemma:
                    lemmas.add(words[-1])
                is_lemma = False
            else:
                is_lemma = True
    print("Total words", len(words))
    random.shuffle(words)
    total_words = len(words)
    train = int(total_words * 0.7)
    val = int(total_words * 0.1)
    test = int(total_words * 0.2)
    fname = os.path.basename(filename)
    fname_without_extension = os.path.splitext(fname)[0]
    with open(fname_without_extension + '_lexeme_rand.train', 'w') as f:
        dump_words(words, f, 0, train)

    with open(fname_without_extension + '_lexeme_rand.val', 'w') as f:
        dump_words(words, f, train, val)

    with open(fname_without_extension + '_lexeme_rand.test', 'w') as f:
        dump_words(words, f, train + val, test)

    with open(fname_without_extension + '_lemma_rand.test', 'w') as f:
        test_words = words[train + val: train + val + test]
        test_lemmas = [w for w in test_words if w in lemmas]
        for word in test_lemmas:
            f.write(word + '\n')

if __name__ == "__main__":
    if len(sys.argv) < 2 or sys.argv[2] == "lexeme":
        mix_lexemes(sys.argv[1], False)
    elif sys.argv[2] == "words":
        mix_words(sys.argv[1])
    elif sys.argv[2] == "lemmas":
        mix_lexemes(sys.argv[1], True)
    else:
        raise Exception("Unknown options:", ' '.join(sys.argv))
