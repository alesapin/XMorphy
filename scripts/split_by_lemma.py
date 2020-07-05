# -*- coding: utf-8 -*-

import sys
from pyxmorphy import MorphAnalyzer, UniSPTag
from enum import Enum
import multiprocessing
from multiprocessing import Pool, Value


class Counter(object):
    def __init__(self):
        self.val = Value('i', 0)

    def increment(self, n=1):
        with self.val.get_lock():
            self.val.value += n
            return self

    @property
    def value(self):
        return self.val.value


class MorphemeLabel(Enum):
    UNKN = 'UNKN'
    PREF = 'PREF'
    ROOT = 'ROOT'
    SUFF = 'SUFF'
    END = 'END'
    LINK = 'LINK'
    HYPH = 'HYPH'
    POSTFIX = 'POSTFIX'
    NONE = None


class Morpheme(object):
    def __init__(self, part_text, label, begin_pos):
        self.part_text = part_text
        self.length = len(part_text)
        self.begin_pos = begin_pos
        self.label = label
        self.end_pos = self.begin_pos + self.length

    def __len__(self):
        return self.length

    def get_labels(self):
        if self.length == 1:
            return ['S-' + self.label.value]
        result = ['B-' + self.label.value]
        result += ['M-' + self.label.value for _ in self.part_text[1:-1]]
        result += ['E-' + self.label.value]
        return result

    def get_simple_labels(self):
        if self.label == MorphemeLabel.SUFF or self.label == MorphemeLabel.PREF or self.label == MorphemeLabel.ROOT:
            result = ['B-' + self.label.value]
            if self.length > 1:
                result += [self.label.value for _ in self.part_text[1:]]
            return result
        else:
            return [self.label.value] * self.length

    def __str__(self):
        return self.part_text + ':' + self.label.value

    @property
    def unlabeled(self):
        return not self.label.value


class Word(object):
    def __init__(self, morphemes=[], speech_part='X'):
        self.morphemes = morphemes
        self.sp = speech_part

    def append_morpheme(self, morpheme):
        self.morphemes.append(morpheme)

    def get_word(self):
        return ''.join([morpheme.part_text for morpheme in self.morphemes])

    def parts_count(self):
        return len(self.morphemes)

    def suffix_count(self):
        return len([morpheme for morpheme in self.morphemes if morpheme.label == MorphemeLabel.SUFFIX])

    def get_labels(self):
        result = []
        for morpheme in self.morphemes:
            result += morpheme.get_labels()
        return result

    def get_simple_labels(self):
        result = []
        for morpheme in self.morphemes:
            result += morpheme.get_simple_labels()
        return result

    def __str__(self):
        return '/'.join([str(morpheme) for morpheme in self.morphemes])

    def __len__(self):
        return sum(len(m) for m in self.morphemes)

    @property
    def unlabeled(self):
        return all(p.unlabeled for p in self.morphemes)


def parse_morpheme(str_repr, position):
    text, label = str_repr.split(':')
    return Morpheme(text, MorphemeLabel[label], position)


def parse_word(str_repr):
    _, word_parts, sp, wordform = str_repr.split('\t')
    parts = word_parts.split('/')
    morphemes = []
    global_index = 0
    for part in parts:
        morphemes.append(parse_morpheme(part, global_index))
        global_index += len(part)
    return Word(morphemes, sp)


def measure_quality(predicted_targets, targets, words):
    TP, FP, FN, equal, total = 0, 0, 0, 0, 0
    SE = ['{}-{}'.format(x, y) for x in "SE" for y in ["ROOT", "PREF", "SUFF", "END", "LINK", "None"]]
    corr_words = 0
    for corr, pred, word in zip(targets, predicted_targets, words):
        corr_len = len(corr)
        pred_len = len(pred)
        boundaries = [i for i in range(corr_len) if corr[i] in SE]
        pred_boundaries = [i for i in range(pred_len) if pred[i] in SE]
        common = [x for x in boundaries if x in pred_boundaries]
        TP += len(common)
        FN += len(boundaries) - len(common)
        FP += len(pred_boundaries) - len(common)
        equal += sum(int(x==y) for x, y in zip(corr, pred))
        total += len(corr)
        corr_words += (corr == pred)
        if corr != pred:
            print("Error in word '{}':\n correct:".format(word.get_word()), corr, '\n!=\n wrong:', pred)

    metrics = ["Precision", "Recall", "F1", "Accuracy", "Word accuracy"]
    results = [TP / (TP+FP), TP / (TP+FN), TP / (TP + 0.5*(FP+FN)),
               equal / total, corr_words / len(targets)]
    return list(zip(metrics, results))


def diff_count(l1, l2):
    result = 0
    for i in range(min(len(l1), len(l2))):
        if l1[i] != l2[i]:
            result += 1
    return result + abs(len(l1) - len(l2))


def read_file(path):
    words = []
    labels = []
    counter = 0
    with open(path, 'r') as test_sample:
        for line in test_sample:
            words.append(parse_word(line.strip()))
            labels.append(words[-1].get_labels())
            counter += 1
            if counter % 1000 == 0:
                print("Loaded", counter)

    return words, labels


total_counter = Counter()
correct_counter = Counter()


def print_counters():
    if total_counter.value % 100 == 0:
        print("Total words", total_counter.value, "correct words", correct_counter.value, "ratio", float(correct_counter.value) / total_counter.value)
        sys.stdout.flush()


def parse_batch(batch):
    words, expected_labels = batch
    analyzer = MorphAnalyzer()
    predicted_labels = []
    for word, expected in zip(words, expected_labels):
        result_forms = analyzer.split_by_lemma(word.get_word(), UniSPTag(word.sp))
        min_predicted = None
        min_predicted_count = 100000000
        total_counter.increment()
        for form in result_forms:
            parts = form.split('/')
            global_index = 0
            labels = []
            for part in parts:
                morpheme = parse_morpheme(part, global_index)
                labels += morpheme.get_labels()
                global_index += len(part)
            if labels == expected:
                predicted_labels.append(labels)
                correct_counter.increment()
                break
            else:
                diff = diff_count(labels, expected)
                if diff < min_predicted_count:
                    min_predicted_count = diff
                    min_predicted = labels
        else:
            if min_predicted is not None:
                predicted_labels.append(min_predicted)
            else:
                predicted_labels.append(['UNKN'] * len(expected))
        print_counters()
    return words, expected_labels, predicted_labels


def chunks(lst, n):
    chunks = []
    for i in range(0, len(lst), n):
        chunks.append(lst[i:i + n])
    return chunks


CHUNKS_COUNT = 8

if __name__ == "__main__":
    words, labels = read_file(sys.argv[1])
    word_chunks = chunks(words, CHUNKS_COUNT)
    labels_chunks = chunks(labels, CHUNKS_COUNT)

    batches = list(zip(word_chunks, labels_chunks))

    pool = Pool(CHUNKS_COUNT)
    results = pool.map(parse_batch, batches)
    pool.close()
    total_words = []
    total_labels = []
    total_predicted = []
    for words, labels, predicted in results:
        total_words += words
        total_labels += labels
        total_predicted += predicted

    print(measure_quality(total_predicted, total_labels, total_words))
