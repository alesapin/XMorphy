#!/usr/bin/env python3

from pyxmorphy import MorphAnalyzer, UniSPTag

def preprocess_verb_lexeme(lexeme):
    # no class, but outdated word
    if lexeme[0] == "нейти":
        return []

    # Three word lexemes
    if lexeme[0] == "таланить":
        return []
    if lexeme[0] == "выведрить":
        return []
    if lexeme[0] == "занездоровиться":
        return []

    if lexeme[0] == "ободневать":
        return []
    # ['познабливать', 'познабливало', 'познабливает']
    if lexeme[0] == "познабливать":
        return []
    # ['лежаться', 'лежалось', 'лежится']
    if lexeme[0] == "лежаться":
        return []
    # ['похолоднеть', 'похолоднеет', 'похолоднело']
    if lexeme[0] == "похолоднеть":
        return []
    # ['вздремнуться', 'вздремнется', 'вздремнулось']
    if lexeme[0] == "вздремнуться":
        return []
    # ['терпеться', 'терпелось', 'терпится']
    if lexeme[0] == "терпеться":
        return []
    # ['поработаться', 'поработалось', 'поработается']
    if lexeme[0] == "поработаться":
        return []
    # ['ветренеть', 'ветренело', 'ветренеет']
    if lexeme[0] == "ветренеть":
        return []
    # ['взглянуться', 'взглянется', 'взглянулось']
    if lexeme[0] == "взглянуться":
        return []
    # ['изжиться', 'изжилось', 'изживется']
    if lexeme[0] == "изжиться":
        return []
    # ['задрематься', 'задремалось', 'задремлется']
    if lexeme[0] == "задрематься":
        return []
    # ['работаться', 'работалось', 'работается']
    if lexeme[0] == "работаться":
        return []
    # ['затошнить', 'затошнит', 'затошнило']
    if lexeme[0] == "затошнить":
        return []
    # ['дождить', 'дождило', 'дождит']
    if lexeme[0] == "дождить":
        return []
    #  ['задышаться', 'задышалось', 'задышится']
    if lexeme[0] == "задышаться":
        return []
    #  ['недужиться', 'недужилось', 'недужится']
    if lexeme[0] == "недужиться":
        return []
    # ['заненаститься', 'заненастится', 'заненастилось']
    if lexeme[0] == "заненаститься":
        return []
    if lexeme[0] == "ведренеть":
        return []
    if lexeme[0] == "подуматься":
        return []
    # ['разъяснеть', 'разъяснеет', 'разъяснело']
    if lexeme[0] == "разъяснеть":
        return []
    # ['всплакнуться', 'всплакнется', 'всплакнулось']
    if lexeme[0] == "всплакнуться":
        return []
    # ['дрематься', 'дремалось', 'дремлется']
    if lexeme[0] == "дрематься":
        return []
    #  ['пролежаться', 'пролежалось', 'пролежится']
    if lexeme[0] == "пролежаться":
        return []
    # ['таланиться', 'таланилось', 'таланится']
    if lexeme[0] == "таланиться":
        return []
    if lexeme[0] == "передуматься":
        return []
    #  ['отхотеться', 'отхотелось', 'отхочется']
    if lexeme[0] == "отхотеться":
        return []
    # ['ободнять', 'ободняет', 'ободняло']
    if lexeme[0] == "ободнять":
        return []
    if lexeme[0] == "проработаться":
        return []
    if lexeme[0] == "пожелаться":
        return []
    # ['счастливиться', 'счастливилось', 'счастливится']
    if lexeme[0] == "счастливиться":
        return []
    # garbage ['пристигнуть', 'пристичь', 'пристиг', 'пристигла', 'пристигло', 'пристигли', 'пристигну', 'пристигнем', 'пристигнешь', 'пристигнете', 'пристигнет', 'пристигнут', 'пристигнем', 'пристигнемте', 'пристигни', 'пристигните']
    if lexeme[0] == "пристигнуть":
        return []
    # garbage ['почесться', 'почтусь', 'почтемся', 'почтешься', 'почтетесь', 'почтется', 'почтутся', 'почтись', 'почтитесь']
    if lexeme[0] == "почесться":
        return []
    # garbage ['восприять', 'восприял', 'восприяла', 'восприяло', 'восприяли']
    if lexeme[0] == "восприять":
        return []
    # ['приубраться', 'приубрался', 'приубралась', 'приубралось', 'приубрались']
    if lexeme[0] == "приубраться":
        return []
    if lexeme[0] == "занедужиться":
        return []
    if len(lexeme) == 3:
        return []

    # ['мястись', 'мятусь', 'мятемся', 'мятешься', 'мятетесь', 'мятется', 'мятутся']
    if lexeme[0] == "мястись":
        return []
    # ['приять', 'приял', 'прияла', 'прияло', 'прияли']
    if lexeme[0] == "приять":
        return []

    if lexeme[0] == "довоеваться":
        return [l for l in lexeme if l != 'довоюемтесь']
    if lexeme[0] == "впрясться":
        return lexeme + ["впрядемся", "впрядешься", "впрядетесь"]
    if lexeme[0] == "выпрясться":
        return lexeme + ["выпрядемся", "выпрядешься", "выпрядетесь"]

    if lexeme[0] == "продремать":
        return [l for l in lexeme if l != 'продремлемте']
    if lexeme[0] == "отрыскать":
        return [l for l in lexeme if "щ" not in l]

    if lexeme[0] == "выгнить":
        return [l for l in lexeme if l != 'выгниемте'] + ['выгнийте', 'выгний']
    if lexeme[0] == "изгнить":
        return [l for l in lexeme if l != "изгниемте"] + ['изгнийте', 'изгний']
    if lexeme[0] == "отгнить":
        return [l for l in lexeme if l != "отгниемте"] + ['отгнийте', 'отгний']
    if lexeme[0] == "ссесться":
        return [l for l in lexeme if l != 'ссядемтесь']

    if lexeme[0] == "живать":
        return [l for l in lexeme if l != "живаают"] + ["живают"]

    if lexeme[0] == "выцедиться":
        lexeme = [l for l in lexeme if l != 'выцедимтесь' and l != 'выцедися'] + ["выцедись"]

    if lexeme[0] == "раскваситься":
        lexeme = [l for l in lexeme if l != 'расквасимтесь'] + ['расквасься', 'расквасьтесь']

    if lexeme[0] == "откашлянуться":
        return [l for l in lexeme if l != 'откашляньтесь'] + ['откашлянитесь']

    if lexeme[0] == "подкашлянуть":
        return [l for l in lexeme if l != 'подкашляньте'] + ['подкашляните']

    if lexeme[0] == "разостлаться":
        return [l for l in lexeme if l not in ("рарасстелешься", "рарасстелетесь", "рарасстелитесь", "расстелемтесь")] + ["расстелитесь", "расстелетесь", "расстелешься"]
    if lexeme[0] == "умилосердить":
        return lexeme + ["умилосердю"]

    if lexeme[0] == "покуролесить":
        return lexeme + ["покуролесь"]
    if lexeme[0] == "закуролесить":
        return lexeme + ["закуролесь"]

    if lexeme[0] == "задудеть":
        return lexeme + ["задудю"]
    if lexeme[0] == "выплестись":
        return [l for l in lexeme if l not in ("выплетемтесь", "выплеться", "выплетьтесь")] +  ['выплетитесь', 'выплетись']
    if lexeme[0] == "поразъехаться":
        return [l for l in lexeme if l not in ("поразъедемтесь", "поразъедься", "поразъедьтесь")] + ["поразъезжайся", "поразъезжайтесь"]

    if lexeme[0] == "уживать":
        return [l for l in lexeme if l != "уживаают"] + ["уживают"]
    if lexeme[0] == "выместись":
        return [l for l in lexeme if l != "выметемтесь" and l != 'выметься' and l != 'выметьтесь'] + ['выметитесь', 'выметись']
    if lexeme[0] == "прорысить":
        return [l for l in lexeme if l not in ("прорысимте", )] + ["прорысю"]
    if lexeme[0] == "сытить":
        return lexeme + ["сытю"]

    if lexeme[0] == "взволочься":
        return [l for l in lexeme if l != "взволочись" and l != 'взволочитесь'] + ["взволокись", "взволокитесь"]
    if lexeme[0] == "исщипать":
        return [l for l in lexeme if 'пл' not in l]

    return lexeme

def gen_convs(lemma, analyzer):
    lexeme = [l.lower() for l in analyzer.generate_lexeme(lemma.upper(), UniSPTag.VERB, False, False, True, True)]
    return lexeme

def get_conv_tails(lemma, inflections, convs_lexeme):
    lemma_tail = inflections[0]
    if lemma_tail:
        common_part = lemma[:-len(lemma_tail)]
    else:
        common_part = lemma

    return [(conv, conv[len(common_part):]) for conv in convs_lexeme]

def generate_conv_tails(classes_dict, analyzer):
    verb_classes = classes_dict["VERB"]
    for class_num, info in verb_classes.items():
        inflections = info["inflections"]
        examples = info["examples"]
        for example in examples:
            if example.endswith('?'):
                example = example[:-1]
            convs = gen_convs(example, analyzer)
            if convs:
                tails = get_conv_tails(example, inflections, convs)
                seen_tails = set()
                for word, tail in tails:
                    if tail in seen_tails:
                        continue
                    seen_tails.add(tail)
                    print(class_num, example, word, tail, sep='\t')
                print()
                break
        else:
            pass
            #print("All example words are non dict class_num {} examples {}".format(class_num, ', '.join(examples)))
