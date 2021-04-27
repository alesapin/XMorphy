#!/usr/bin/env python3
import json
import re
import sys

NOUNS_ANCHOR = "NFLX xl_nflx[] = // NCLASS"
ADJF_ANCHOR = "AFLX xl_aflx[] = // ACLASS"
ADJS_ANCHOR = "ASFLX xl_sflx[] = // ASCLASS"
VERB_ANCHOR = "VFLX xl_vflx[] = // VCLASS"

NEW_CLASS_ANCHOR = "/*"

CLASS_NUMBER_RE = re.compile(r"[0-9]+")
EXAMPLE_RE = re.compile(r"/\* *[а-яА-ЯёЁ0-9\-A-Z><{},?\/a-z ]+ *\*/")
NOUN_GEN_TAG_RE = re.compile(r"GN_UNKN|MASC|FEM|NEUT|COMN")
NOUN_ANIM_TAG_RE = re.compile(r"ANI_NO|ANI_YES|ANI_INDIF|ANI_INDIF")

ADJ_CLASS_INNER_RE = re.compile(r"{[^{}]+}")

def cut_example(example_match):
    if example_match.startswith('/*'):
        example_match = example_match[2:]
    if example_match.endswith('*/'):
        example_match = example_match[:-2]

    return example_match.strip().split(",")

def parse_noun_class(text):
    num_matches = CLASS_NUMBER_RE.search(text)
    class_num = int(num_matches.group())
    example_matches = EXAMPLE_RE.findall(text)
    examples = cut_example(example_matches[1])
    gen_tag_match = NOUN_GEN_TAG_RE.search(text)
    gen_tag = gen_tag_match.group()
    anim_tag_match = NOUN_ANIM_TAG_RE.search(text)
    anim_tag = anim_tag_match.group()
    all_classes = ADJ_CLASS_INNER_RE.findall(text)
    classes = all_classes[0].replace('"', '').replace('{', '').replace('}', '').replace(' ', '').split(',')
    return class_num, {
        "tags" : [gen_tag, anim_tag],
        "inflections": classes,
        "examples": examples
    }

def build_nouns_dict(itr):
    itr.readline() # skip first
    current_class_text = ""
    result = {}
    for line in itr:
        if '};' in line:
            num, dct = parse_noun_class(current_class_text)
            result[num] = dct
            break
        if '//' in line:
            continue
        if line.startswith(NEW_CLASS_ANCHOR):
            if current_class_text:
                num, dct = parse_noun_class(current_class_text)
                result[num] = dct
            current_class_text = line.strip()
        else:
            current_class_text += line.strip()
    return result

def parse_adj_class(text):
    num_matches = CLASS_NUMBER_RE.search(text)
    class_num = int(num_matches.group())
    example_matches = EXAMPLE_RE.findall(text)
    if len(example_matches) > 1:
        examples = cut_example(example_matches[1])
    else:
        examples = []
    all_classes = ADJ_CLASS_INNER_RE.findall(text)
    classes = []
    for cls in all_classes:
        cls_array = cls[1:-1].replace('"', '').replace(' ', '').split(',')
        if not all(c == "" for c in cls_array):
            classes += cls_array

    return class_num, {
        "tags" : [],
        "inflections": classes,
        "examples": examples
    }


def build_adj_dict(itr):
    itr.readline() # skip first
    current_class_text = ""
    result = {}
    for line in itr:
        if '};' in line:
            num, dct = parse_adj_class(current_class_text)
            result[num] = dct
            break
        if '//' in line:
            continue
        if line.startswith(NEW_CLASS_ANCHOR):
            if current_class_text and 'Pascal counts from 1' not in current_class_text:
                num, dct = parse_adj_class(current_class_text)
                result[num] = dct
            current_class_text = line.strip()
        else:
            current_class_text += line.strip()
    return result

def parse_short_adj_class(text):
    num_matches = CLASS_NUMBER_RE.search(text)
    class_num = int(num_matches.group())
    example_matches = EXAMPLE_RE.findall(text)
    if len(example_matches) > 1:
        examples = [cut_example(e) for e in example_matches[1:]]
    else:
        examples = []

    all_classes = ADJ_CLASS_INNER_RE.findall(text)
    classes = []
    for cls in all_classes:
        cls_array = cls[1:-1].replace('"', '').replace(' ', '').split(',')
        classes.append(cls_array)

    result = []

    examples_counter = 0
    for i, cls in enumerate(classes):
        if not all(c == "" for c in cls):
            result.append((str(class_num) + "-" + str(i), {"tags": [], "inflections": cls, "examples": examples[examples_counter]}))
            examples_counter += 1
    return result



def build_short_adj_dict(itr):
    itr.readline() # skip first
    current_class_text = ""
    result = {}
    for line in itr:
        if '};' in line:
            current_results = parse_short_adj_class(current_class_text)
            for num, dct in current_results:
                result[num] = dct
            break
        if '//' in line:
            continue
        if line.startswith(NEW_CLASS_ANCHOR):
            if current_class_text and 'Pascal counts from 1' not in current_class_text:
                current_results = parse_short_adj_class(current_class_text)
                for num, dct in current_results:
                    result[num] = dct
            current_class_text = line.strip()
        else:
            current_class_text += line.strip()
    return result


if __name__ == "__main__":
    classes_file = sys.argv[1]
    common_result = {}

    with open(classes_file, 'r') as read_file:
        for line in read_file:
            if NOUNS_ANCHOR in line:
                common_result["NOUN"] = build_nouns_dict(read_file)
            elif ADJF_ANCHOR in line:
                common_result["ADJF"] = build_adj_dict(read_file)
            elif ADJS_ANCHOR in line:
                common_result["ADJS"] = build_short_adj_dict(read_file)
            elif VERB_ANCHOR in line:
                common_result["VERB"] = build_adj_dict(read_file)

        print(json.dumps(common_result, ensure_ascii=False, indent=4, sort_keys=True))
