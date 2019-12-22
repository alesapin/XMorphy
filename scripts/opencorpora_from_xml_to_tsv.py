#!/usr/bin/env python
# -*- coding: utf-8 -*-

import xml.etree.ElementTree as ET
import sys

MAIN_TAGS = {
    "anim": "ANim",
    "inan": "ANim",
    "masc": "GNdr",
    "femn": "GNdr",
    "neut": "GNdr",
    "ms-f": "GNdr",
    "sing": "NMbr",
    "plur": "NMbr",
    "nomn": "CAse",
    "gent": "CAse",
    "datv": "CAse",
    "accs": "CAse",
    "ablt": "CAse",
    "loct": "CAse",
    "voct": "CAse",
    "gen1": "CAse",
    "gen2": "CAse",
    "acc2": "CAse",
    "loc1": "CAse",
    "loc2": "CAse",
    "perf": "ASpc",
    "impf": "ASpc",
    "tran": "TRns",
    "intr": "TRns",
    "1per": "PErs",
    "2per": "PErs",
    "3per": "PErs",
    "pres": "TEns",
    "past": "TEns",
    "futr": "TEns",
    "indc": "MOod",
    "impr": "MOod",
    "incl": "INvl",
    "excl": "INvl",
    "actv": "VOic",
    "pssv": "VOic",

}
AUXILARY_TAGS = {
    "Sgtm",
    "Pltm",
    "Fixd",
    "Abbr",
    "Apro",
    "Name",
    "Surn",
    "Patr",
    "Geox",
    "Orgn",
    "Trad",
    "Subx",
    "Supr",
    "Qual",
    "Apro",
    "Anum",
    "Poss",
    "V-ey",
    "V-oy",
    "Cmp2",
    "V-ej",
    "Impe",
    "Impx",
    "Mult",
    "Refl",
    "Infr",
    "Slng",
    "Arch",
    "Litr",
    "Erro",
    "Dist",
    "Ques",
    "Dmns",
    "Prnt",
    "V-be",
    "V-en",
    "V-ie",
    "V-bi",
    "Fimp",
    "Prdx",
    "Coun",
    "Coll",
    "V-sh",
    "Af-p",
    "Inmx",
    "Vpre",
    "Anph",
    "Init",
    "Adjx",
    "Ms-f",
    "Hypo",
}


# <token id="2" text="Школа">
#     <tfr rev_id="834910" t="Школа"> <!-- source word -->
#         <v>
#            <l id="380220" t="школа"> <!-- word lemma -->
#                <g v="NOUN"/> <!-- part of speech -->
#                <g v="inan"/> <!-- animality      -->
#                <g v="femn"/> <!-- gender         -->
#                <g v="sing"/> <!-- number         -->
#                <g v="nomn"/> <!-- case           -->
#            </l>
#         </v>
#     </tfr>
# </token>


def format_main_tag(tag_list):
    result = []
    for tag in tag_list:
        if tag in MAIN_TAGS:
            result.append(MAIN_TAGS[tag] + '=' + tag)
    if not result:
        return '_'
    else:
        return '|'.join(result)


def format_aux_tag(tag_list):
    result = []
    for tag in tag_list:
        if tag in AUXILARY_TAGS:
            result.append(tag)

    if not result:
        return '_'
    else:
        return '|'.join(result)


def convert_single_token(token):
    tfr_tag = token[0]
    source_word = tfr_tag.attrib['t']
    lemma_tag = tfr_tag[0][0]
    lemma_word = lemma_tag.attrib['t']
    tag_list = []
    for tag in lemma_tag:
        tag_list.append(tag.attrib['v'])
    part_of_speech = tag_list[0]
    tag_list = tag_list[1:]

    return '\t'.join(
        [source_word, lemma_word, part_of_speech,
         format_main_tag(tag_list), format_aux_tag(tag_list)])


def convert_sentence(sentence):
    tokens = sentence.find("tokens")
    result = ""
    for index, token in enumerate(tokens):
        result += str(index + 1) + '\t' + convert_single_token(token) + '\n'
    return result


def convert_text(text):
    result = ""
    paragraphs = text.find("paragraphs")
    for paragraph in paragraphs:
        for sentence in paragraph:
            sentence_text = convert_sentence(sentence)
            if sentence_text:
                result += sentence_text + '\n'

    return result


# Script allows to convert opencorpora labeled
# corpus in .xml format to simple tab-separated file
if __name__ == "__main__":
    tree = ET.parse(sys.argv[1])
    root = tree.getroot()
    for text in root:
        print(convert_text(text), end='')
