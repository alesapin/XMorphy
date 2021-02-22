#!/usr/bin/env python3
from enum import Enum
import copy

class MorphemeLabel(Enum):
    UNKN = 'UNKN'
    PREF = 'PREF'
    ROOT = 'ROOT'
    SUFF = 'SUFF'
    END = 'END'
    LINK = 'LINK'
    HYPH = 'HYPH'
    POSTFIX = 'POSTFIX'
    NUMB = 'NUMB'
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

    def add_letter(self, letter):
        self.part_text += letter
        self.length = len(self.part_text)
        self.end_pos = self.begin_pos + self.length

    def pop_letter(self):
        if self.part_text:
            self.part_text = self.part_text[:-1]
            self.length = len(self.part_text)
            self.end_pos = self.begin_pos + self.length

    def get_label(self):
        return self.label.value

    def set_letter(self, pos, letter):
        lst = list(self.part_text)
        lst[pos] = letter
        self.part_text = ''.join(lst)

    def get_label_decorated(self, pos):
        if pos >= self.length:
            raise Exception("Cannot get label at pos {}".format(pos))
        if self.length == 1:
            return 'S-' + self.label.value

        if pos == 0:
            return 'B-' + self.label.value
        elif pos == self.length - 1:
            return 'E-' + self.label.value
        else:
            return 'M-' + self.label.value

    def get_labels(self):
        if self.length == 1:
            return ['S-' + self.label.value]
        result = ['B-' + self.label.value]
        result += ['M-' + self.label.value for _ in self.part_text[1:-1]]
        result += ['E-' + self.label.value]
        return result

    def get_simple_labels(self):
        if (self.label == MorphemeLabel.SUFF or self.label == MorphemeLabel.PREF or self.label == MorphemeLabel.ROOT):

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
    def __init__(self, morphemes=[]):
        self.morphemes = morphemes

    def append_letter(self, letter, label, new):
        if self.morphemes and not new:
            if label == self.morphemes[-1].label:
                self.morphemes[-1].add_letter(letter)
                return
        self.morphemes.append(Morpheme(letter, label, 0))

    def pop_letter(self):
        if self.morphemes:
            self.morphemes[-1].pop_letter()
            if len(self.morphemes[-1]) == 0:
                self.morphemes.pop()

    def copy(self):
        return copy.deepcopy(self)

    def append_morpheme(self, morpheme):
        self.morphemes.append(morpheme)

    def get_word(self):
        return ''.join([morpheme.part_text for morpheme in self.morphemes])

    def parts_count(self):
        return len(self.morphemes)

    def get_label_decorated(self, pos):
        global_pos = 0
        for morpheme in self.morphemes:
            for i in range(len(morpheme.part_text)):
                if global_pos == pos:
                    return morpheme.get_label_decorated(i)
                global_pos += 1
        raise Exception("No such position", pos, str(self))

    def get_label(self, pos):
        global_pos = 0
        for morpheme in self.morphemes:
            for _ in morpheme.part_text:
                if global_pos == pos:
                    return morpheme.get_label()
                global_pos += 1

        raise Exception("No such position", pos, str(self))

    def get_last_label_value(self):
        return self.morphemes[-1].label

    def get_letter(self, pos):
        global_pos = 0
        for morpheme in self.morphemes:
            for letter in morpheme.part_text:
                if global_pos == pos:
                    return letter
                global_pos += 1

        raise Exception("No such position", pos, str(self))

    def set_letter(self, pos, letter):
        global_pos = 0
        for morpheme in self.morphemes:
            for i in range(len(morpheme.part_text)):
                if global_pos == pos:
                    morpheme.set_letter(i, letter)
                    return
                global_pos += 1

        raise Exception("No such position", pos, str(self))


    def set_label(self, pos, label):
        global_pos = 0
        for morpheme in self.morphemes:
            for i in range(len(morpheme.part_text)):
                if global_pos == pos:
                    morpheme.label = label
                    return
                global_pos += 1

        raise Exception("No such position", pos, str(self))

    def suffix_count(self):
        return len([morpheme for morpheme in self.morphemes
                    if morpheme.label == MorphemeLabel.SUFF])

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


def parse_word(wordform, parse):
    if ':' in wordform or '/' in wordform:
        return Word([Morpheme(wordform, MorphemeLabel['UNKN'], 0)])

    parts = parse.split('/')
    morphemes = []
    global_index = 0
    for part in parts:
        morphemes.append(parse_morpheme(part, global_index))
        global_index += len(part)
    return Word(morphemes)


class MorphemeClass(object):
    def __init__(self, speech_part, class_num):
        self.speech_part = speech_part
        self.class_num = str(class_num)

    def __str__(self):
        return "(sp: " + self.speech_part + ", class_num: " + str(self.class_num) + ")"

class TailPart(object):
    def __init__(self, text, new, last):
        self.text = text
        self.new = new
        self.last = last

def parse_tail_parts(tail):
    result = []
    new = False
    last = False
    tail_len = 0
    current = ''
    for letter in tail:
        if letter == '-':
            if current:
                result.append(TailPart(current, new, last))
                current = ''
            new = True
        else:
            tail_len += 1
            current += letter

    if current:
        result.append(TailPart(current, new, last))
    if result:
        result[-1].last = True
    return result, tail_len
