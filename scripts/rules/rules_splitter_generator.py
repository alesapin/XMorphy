#!/usr/bin/env python3
import json
import sys
import os
from pyxmorphy import MorphAnalyzer, UniSPTag
import copy
import random
from enum import Enum
from morpheme_utils import Morpheme, MorphemeLabel, Word, parse_word, MorphemeClass
from morpheme_utils import parse_tail_parts, TailPart
from noun_utils import gen_multi_lexeme_parse, preprocess_noun_lexeme

analyzer = MorphAnalyzer()

def get_forms_tails(forms):
    prefix = os.path.commonprefix(forms)
    return [x[len(prefix):] for x in forms]

def find_inflection_match(tails, inflection_class, verbose=False):
    max_match_len = 0
    inflection_class_set = set(inflection_class)
    tails_set = set(tails)
    common = inflection_class_set & tails_set
    if not inflection_class_set.issubset(tails_set):
        return 0
    if not inflection_class[0] == tails[0]:
        return 0
    if len(common) == 1:
        return 0
    common_inflections = [infl for infl in inflection_class if infl in common]
    if len(common_inflections) > max_match_len:
        max_match_len = len(common_inflections)
    return max_match_len

def get_class_match(tails, classes_dict, sp):
    max_match_len = 0
    max_sp = None
    max_class_num = 0
    for class_num, cls in classes_dict[sp].items():
        match_len = find_inflection_match(tails, cls["inflections"])
        if match_len > max_match_len:
            max_sp = sp
            max_class_num = class_num
            max_match_len = match_len

    return MorphemeClass(max_sp, max_class_num)

def try_to_find_class(lemma, tails, cross_dict, classes_dict, sp):
    if lemma in cross_dict:
        found_sp, class_num = cross_dict[lemma]
        if found_sp == sp:
            return MorphemeClass(sp, class_num)
    match = get_class_match(tails, classes_dict, sp)
    if match is None or match.speech_part is None:
        return None
    return match

def load_classes(path):
    with open(path, 'r') as p:
        classes = json.load(p)
        for k in classes['NOUN']:
            classes['NOUN'][k]['inflections'] = classes['NOUN'][k]['inflections'][:6]

        classes_labeled = copy.deepcopy(classes)
        for sp in classes_labeled:
            for k in classes_labeled[sp]:
                classes_labeled[sp][k]["inflections"] = [(f.replace('-', ''), f[:-1]) if f.endswith('-') else (f.replace('-', ''), f) for f in classes_labeled[sp][k]["inflections"]]

        classes_non_labeled = copy.deepcopy(classes)
        for sp in classes_non_labeled:
            for k in classes_non_labeled[sp]:
                for i in range(len(classes_non_labeled[sp][k]["inflections"])):
                    classes_non_labeled[sp][k]["inflections"][i] = classes_non_labeled[sp][k]["inflections"][i].replace('-', '')

        return classes_labeled, classes_non_labeled

def load_tikhonov_dict(path):
    result = []
    with open(path, 'r') as p:
        for line in p:
            word, parse = line.strip().split('\t')
            result.append(parse_word(word, parse))
    return result

def load_cross_lexica_dict(path):
    speech_part_codes = {
        1 : 'NOUN',
        2 : 'VERB',
        3 : 'ADJF',
    }
    result = {}
    with open(path, 'r') as p:
        for line in p:
            sp, cls, un1, un2, word = line.strip().split()
            if int(sp) == 1:
                result[word] = (speech_part_codes[int(sp)], int(cls))
    return result

def get_lexeme(lemma, speech_part, only_single, only_mult):
    lexeme = [l.lower() for l in analyzer.generate_lexeme(lemma.upper(), speech_part, only_single, only_mult)]

    if not lexeme:
        return []

    if speech_part == UniSPTag.NOUN:
        return preprocess_noun_lexeme(lexeme, lemma, only_single, only_mult)

    return lexeme

def gen_parse2(common_part, tail, lemma, lemma_parse):
    parsed_lemma = parse_word(lemma, lemma_parse)
    common_parse = parsed_lemma.copy()
    is_polden = lemma == "полдень"
    has_postfix = False
    if parsed_lemma.morphemes[-1].label == MorphemeLabel.POSTFIX:
        has_postfix = True

    tail_parts, tail_len = parse_tail_parts(tail)
    postfix_part = None

    if has_postfix:
        postfix_part = tail_parts[-1]
        tail_parts.pop()

    while len(common_parse) > len(common_part):
        common_parse.pop_letter()

    if not tail_parts:
        return str(common_parse)

    current_pos = len(common_parse)
    if current_pos >= len(lemma):
        if len(tail_parts) > 1:
            raise Exception("Unknown case")
        new = False
        for letter in tail_parts[0].text:
            common_parse.append_letter(letter, MorphemeLabel.END, new)
            new = True

        if has_postfix:
            new = False
            for letter in postfix_part.text:
                common_parse.append_letter(letter, MorphemeLabel.POSTFIX, new)
                new = False

        return str(common_parse)

    if parsed_lemma.get_label(current_pos) != 'ROOT':
        complete_root = True
    elif lemma[current_pos] == 'ь' and (len(parsed_lemma) == current_pos + 1 or parsed_lemma.get_label(current_pos + 1) != 'ROOT'):
        if tail_parts[0].text[0] != 'ь' or len(tail_parts[0].text[0]) > 1:
            complete_root = True
        else:
            complete_root = False
    else:
        complete_root = False

    current_tail_pos = 0
    if not complete_root and not tail_parts[current_tail_pos].new:
        for letter in tail_parts[current_tail_pos].text:
            common_parse.append_letter(letter, MorphemeLabel.ROOT, False)
        current_tail_pos += 1

    while current_tail_pos < len(tail_parts) - 1:
        new = tail_parts[current_tail_pos].text[0] != 'ь'
        label = MorphemeLabel.SUFF
        if is_polden:
            label = MorphemeLabel.ROOT
        for letter in tail_parts[current_tail_pos].text:
            common_parse.append_letter(letter, label, new)
            new = False
        current_tail_pos += 1

    if current_tail_pos < len(tail_parts):
        label = MorphemeLabel.END
        if not tail_parts[current_tail_pos].new and not tail_parts[current_tail_pos].text == 'ь':
            label = MorphemeLabel.SUFF

        new = True
        for letter in tail_parts[current_tail_pos].text:
            common_parse.append_letter(letter, label, new)
            new = False

    if has_postfix:
        new = False
        for letter in postfix_part.text:
            common_parse.append_letter(letter, MorphemeLabel.POSTFIX, new)
            new = False

    if len(common_parse) != len(common_part) + tail_len:
        print("Common", common_parse)
        raise Exception("Incorrect parse generated")

    return str(common_parse)


def generate_nouns_parses(tikhonov, cross_lexica, classes_dict, classes_labels):

    def process_lexeme(single_number, multiple_number, distinct_classes):
        distinct = {}
        lexemes = {}
        for word in tikhonov:
            word_text = word.get_word()
            lexeme = get_lexeme(word_text, UniSPTag.NOUN, single_number, multiple_number)
            if not lexeme:
                continue
            lexemes[word_text] = (lexeme, word)
        items = list(lexemes.items())
        counter = 0
        for lemma, (lexeme, word) in items:
            counter += 1
            printed = False
            if len(lexeme) != 1:
                tails = get_forms_tails(lexeme)
                got_class = try_to_find_class(lexeme[0], tails, cross_lexica, classes_dict, str(UniSPTag.NOUN))
                if not got_class:
                    raise Exception("Cannot find class for", lemma, lexeme)

                inflections = classes_labels[got_class.speech_part][got_class.class_num]["inflections"]

                if multiple_number:
                    lemma_parse = gen_multi_lexeme_parse(lemma, str(word), lexeme[0], inflections[0][1])
                else:
                    lemma_parse = str(word)

                #if got_class.class_num not in distinct:
                printed = True
                distinct[got_class.class_num] = []

                for (inflection, dashed_inflection) in inflections:
                    if inflections[0][0]:
                        wordform = lexeme[0][:-len(inflections[0][0])]
                    else:
                        wordform = lexeme[0]
                    new_parse = gen_parse2(wordform, dashed_inflection, lexeme[0], lemma_parse)
                    wordform += inflection
                    if not distinct_classes:
                        print(wordform, '\t', new_parse, '\t', dashed_inflection, '\t', got_class)

                    if printed:
                        distinct[got_class.class_num].append('\t'.join([wordform, new_parse, dashed_inflection, str(got_class)]))
            else:
                if not distinct_classes:
                    print(lemma, '\t', word, '\t', '\t', "UNKN")
            if not distinct_classes:
                print()
        if distinct_classes:
            for _, lexeme in sorted(distinct.items(), key=lambda x: int(x[0])):
                print('\n'.join(lexeme))
                print()

    process_lexeme(True, False, True)
    process_lexeme(False, True, True)


if __name__ == "__main__":
    classes_json = sys.argv[1]
    classes_labels, classes_dict = load_classes(classes_json)
    tikhonov_dict = sys.argv[2]
    tikhonov = load_tikhonov_dict(tikhonov_dict)
    cross_lexica_dict = sys.argv[3]
    cross_lexica = load_cross_lexica_dict(cross_lexica_dict)

    assert gen_parse2("передн", "-ей", "передняя", "перед:ROOT/н:SUFF/яя:END") == "перед:ROOT/н:SUFF/ей:END"
    assert gen_parse2("барыш", "ень", "барышни", "бар:ROOT/ыш:SUFF/н:SUFF/и:END") == "бар:ROOT/ыш:SUFF/ень:SUFF"
    assert gen_parse2("погреб", "ц-а", "погребец", "погреб:ROOT/ец:SUFF") == "погреб:ROOT/ц:SUFF/а:END"
    assert gen_parse2("брем", "ен-и", "бремя", "брем:ROOT/я:END") == "брем:ROOT/ен:SUFF/и:END"
    assert gen_parse2("доч", "ер-и", "дочь", "дочь:ROOT") == "доч:ROOT/ер:SUFF/и:END"
    assert gen_parse2("нижеподписавш", "-его-ся", "нижеподписавшийся", "ниж:ROOT/е:LINK/под:PREF/пис:ROOT/а:SUFF/вш:SUFF/ий:END/ся:POSTFIX") == "ниж:ROOT/е:LINK/под:PREF/пис:ROOT/а:SUFF/вш:SUFF/его:END/ся:POSTFIX", gen_parse("нижеподписавшегося", "-его-ся", "нижеподписавшийся", "-ий-ся", "ниж:ROOT/е:LINK/под:PREF/пис:ROOT/а:SUFF/вш:SUFF/ий:END/ся:POSTFIX")
    assert gen_parse2("тюр", "ьм-ах", "тюрьмы", "тюрьм:ROOT/а:END") == "тюрьм:ROOT/ах:END"
    assert gen_parse2("шан", "ег", "шаньга", "шаньг:ROOT/а:END") == "шанег:ROOT"
    assert gen_parse2("марсиан", "", "марсианин", "марс:ROOT/и:SUFF/ан:SUFF/ин:SUFF") == "марс:ROOT/и:SUFF/ан:SUFF"
    assert gen_parse2("стрем", "ен-а", "стремя", "стрем:ROOT/я:END") == "стрем:ROOT/ен:SUFF/а:END"
    assert gen_parse2("м", "х-а", "мох", "мох:ROOT") == "мх:ROOT/а:END"
    assert gen_parse2("автовладел", "ь-ц-а", "автовладелец", "авто:ROOT/влад:ROOT/е:SUFF/л:SUFF/ец:SUFF") == "авто:ROOT/влад:ROOT/е:SUFF/ль:SUFF/ц:SUFF/а:END"
    assert gen_parse2("господ", "ь", "господь", "господь:ROOT") == "господь:ROOT"
    assert gen_multi_lexeme_parse("рубчик", "руб:ROOT/ч:SUFF/ик:SUFF", "рубчики", "-и") == "руб:ROOT/ч:SUFF/ик:SUFF/и:END"
    assert gen_multi_lexeme_parse("уголек", "угол:ROOT/ек:SUFF", "угольки", "-и") == "угол:ROOT/ьк:SUFF/и:END"
    assert gen_multi_lexeme_parse("друг", "друг:ROOT", "друзья", "ь-я") == "друзь:ROOT/я:END"
    assert gen_multi_lexeme_parse("бремя", "брем:ROOT/я:END", "бремена", "-а") == "брем:ROOT/ен:SUFF/а:END"
    assert gen_parse2("шпул", "ь", "шпули", "шпул:ROOT/и:END") == "шпул:ROOT/ь:END"

    generate_nouns_parses(tikhonov, cross_lexica, classes_dict, classes_labels)
