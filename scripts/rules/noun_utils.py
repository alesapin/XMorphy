#!/usr/bin/env python3

from morpheme_utils import parse_word, Word, MorphemeLabel, parse_tail_parts

def gen_multi_lexeme_parse(lemma, lemma_parse, multform, multform_tail):
    parsed_lemma = parse_word(lemma, lemma_parse)
    multform_parse = parsed_lemma.copy()

    if multform == "люди":
        return "люд:ROOT/и:END"
    if len(lemma) == len(multform):
        for i in range(len(multform)):
            multform_parse.set_letter(i, multform[i])
        return str(multform_parse)

    tail_parts, tail_len = parse_tail_parts(multform_tail)

    if len(multform) > len(lemma):
        if len(tail_parts) == 1 and multform[:-len(tail_parts[0].text)] == lemma:
            new = True
            for letter in tail_parts[0].text:
                multform_parse.append_letter(letter, MorphemeLabel.END, new)
                new = False
            return str(multform_parse)

        new = True
        for i in range(len(multform) - tail_len):
            if i < len(multform_parse):
                if multform_parse.get_label(i) != 'END':
                    multform_parse.set_letter(i, multform[i])
                else:
                    multform_parse.set_letter(i, multform[i])
                    multform_parse.set_label(i, MorphemeLabel.SUFF)
                    new = False
            else:
                multform_parse.append_letter(multform[i], MorphemeLabel.SUFF, new)
                new = False

        while len(multform_parse) > len(multform) - tail_len:
            multform_parse.pop_letter()

        current_tail_pos = 0
        while current_tail_pos < len(tail_parts) - 1:
            if tail_parts[current_tail_pos].text == 'ь':
                new = False
                label = multform_parse.get_last_label_value()
            else:
                label = MorphemeLabel.SUFF
                new = tail_parts[current_tail_pos].text[0] != 'ь'

            for letter in tail_parts[current_tail_pos].text:
                multform_parse.append_letter(letter, label, new)
                new = False
            current_tail_pos += 1

        if current_tail_pos < len(tail_parts):
            label = MorphemeLabel.END
            if not tail_parts[current_tail_pos].new:
                label = MorphemeLabel.SUFF

            new = True
            for letter in tail_parts[current_tail_pos].text:
                multform_parse.append_letter(letter, label, new)
                new = False
        return str(multform_parse)

    return str(lemma_parse)

def preprocess_noun_lexeme(lexeme, lemma, only_single, only_mult):
    if lexeme[0] == "детки":
        lexeme = [l for l in lexeme if not l.startswith("ребеноч")]

    if lexeme[0] == "дитя":
        lexeme = [l for l in lexeme if not l.startswith("дитят")]

    if lexeme[0] == "сивучата":
        lexeme = [l for l in lexeme if not l.startswith("сивучон")]

    if lexeme[0] == "люди":
        lexeme = [l for l in lexeme if not l.startswith("человек")]

    if lexeme[0] == "боголюди":
        lexeme = [l for l in lexeme if not l.startswith("богочеловек")]

    if lexeme[0] == "куличата":
        lexeme = [l for l in lexeme if not l.startswith("куличон")]

    if lexeme[0] == "любовь":
        lexeme = [l for l in lexeme if not l in ('любови', )]

    if lexeme[0] == "двери":
        lexeme = [l for l in lexeme if not l in ('дверьми', )]

    if lexeme[0] == "пяльцы":
        lexeme = [l for l in lexeme if not l in ('пялец', )]

    # рыльца
    if lexeme[0].endswith('ьца'):
        lexeme = [l for l in lexeme if not l.endswith('ец')]

    if lexeme[0] == "пешни":
        lexeme = [l for l in lexeme if not l in ('пешен', )]

    if lexeme[0] == "лютни":
        lexeme = [l for l in lexeme if not l in ('лютен', )]

    if lexeme[0] == "полымя":
        lexeme = [lexeme[0]]

    if lexeme[0] == "ветер":
        lexeme = [l for l in lexeme if l != 'ветр']

    if lexeme[0] == "огонь":
        lexeme = [l for l in lexeme if l != 'огнь']

    if lemma == "борок":
        lexeme = [l for l in lexeme if not l.startswith('борк')]

    if lexeme[0] == "человек":
        lexeme = [l for l in lexeme if l != "человече"]

    if lemma == "медок":
        lexeme = [l for l in lexeme if l not in ("медока", "медоку", "медоке", "медоков", "медокам", "медоками", "медоках")]

    if lexeme[0] == "дяди":
        lexeme = [l for l in lexeme if l not in ("дядьев", "дядьям", "дядьями", "дядьях",)]

    if lexeme[0] == "сопла":
        lexeme = [l for l in lexeme if l != "сопел"]

    if lexeme[0] == "блюдца":
        lexeme = [l for l in lexeme if l != "блюдцев"]

    if lexeme[0] == "имя":
        lexeme = [l for l in lexeme if l not in ("им", )]

    if lemma == "брелок":
        lexeme = [l for l in lexeme if not l.startswith("брелк")]

    if lemma == "тонкотел":
        lexeme = [l for l in lexeme if not l.startswith("тонкотл")]

    if lexeme[0] == "слава":
        lexeme = [l for l in lexeme if l != "слав"]

    if lexeme[0] == "соня":
        lexeme = [l for l in lexeme if l != "сонь"]

    if lexeme[0] == "бобер":
        lexeme = [l for l in lexeme if l != "бобр"]

    if lexeme[0] == "тысяча":
        lexeme = [l for l in lexeme if l != "тысячью"]

    if lexeme[0] == "катюша":
        lexeme = [l for l in lexeme if l != "катюш"]

    if lexeme[0] == "дина":
        lexeme = [l for l in lexeme if l != "дин"]

    if lexeme[0] == "ноль-ноль":
        lexeme = [l for l in lexeme if "-нл" not in l]

    if lexeme[0] == "батька":
        lexeme = [l for l in lexeme if l != "батько"]

    if lexeme[0] == "санорин":
        lexeme = [l for l in lexeme if l != "санориным"]

    if lexeme[0] == "мак":
        lexeme = ["мак"] + [l for l in lexeme if l != "мак"]

    if lexeme[0] == "мать":
        lexeme = [l for l in lexeme if l != 'матерь']

    if lexeme[0] == "песня":
        lexeme = [l for l in lexeme if l not in ('песнь', 'песнью')]

    if lexeme[0] == "политдень":
        lexeme = [l for l in lexeme if l not in ('политденя', 'политденю', 'политденем', 'политдене')]

    if lemma == "скерцино":
        lexeme = [l for l in lexeme if l not in ('скерцину', 'скерцине', 'скерцина')]

    if lemma == "простыня":
        lexeme = [l for l in lexeme if l not in ('простынь', 'простынь', 'простынью')]

    if lemma == "приятность":
        lexeme = [l for l in lexeme if l not in ('приятностию', )]

    if lemma == "твердость":
        lexeme = [l for l in lexeme if l not in ('твердостию', )]

    if lemma == "артогонь":
        lexeme = [l for l in lexeme if l not in ('артогнь', )]

    if lemma.endswith("-день"):
        lexeme = [l for l in lexeme if not l.endswith('-деня') and not l.endswith('-деню') and not l.endswith('-денем') and not l.endswith('-дене')]

    if lexeme[0].endswith("-дни"):
        lexeme = [l for l in lexeme if not l.endswith('-деней') and not l.endswith('-деням') and not l.endswith('-денями') and not l.endswith('-денях')]

    if lexeme[0] == "политдни":
        lexeme = [l for l in lexeme if l not in ('политденей', 'политденям', 'политденями', 'политденях')]

    if lexeme[0] == "мулята":
        lexeme = [l for l in lexeme if 'енк' not in l]

    if lexeme[0] == "языки":
        lexeme = [l for l in lexeme if l not in ("языцех", )]

    if lexeme[0] == "супругов":
        lexeme.remove("супруги")
        lexeme = ["супруги"] + lexeme

    if lemma == "наем":
        lexeme = [l for l in lexeme if l not in ('найм', )]

    if lemma == "госпожа":
        lexeme = [l for l in lexeme if l not in ('госпожей', )]

    if lemma == "господь":
        lexeme = [l for l in lexeme if l not in ('господи', )]

    if lemma == "комель":
        lexeme = [l for l in lexeme if l not in ('комля', 'комлю', 'комлем', 'комле')]

    if lemma == "кочан":
        lexeme = [l for l in lexeme if l not in ('кочна', 'кочну', 'кочне')]

    if lemma == "круча":
        lexeme = [l for l in lexeme if l not in ('кручью', )]

    if lemma == "часть":
        lexeme = [l for l in lexeme if l not in ('частию', )]

    if lemma == "рубин":
        lexeme = [l for l in lexeme if l not in ('рубиным', )]

    if lemma == "литвин":
        lexeme = [l for l in lexeme if l not in ('литвиным', )]

    if lexeme[0] == "ветра":
        lexeme = [l for l in lexeme if l not in ('ветры', )]

    if lexeme[0] == "донца":
        lexeme = [l for l in lexeme if l not in ('донцев', )]

    if lexeme[0] == "культсектора":
        lexeme = [l for l in lexeme if l not in ('культсекторы', )]

    if lexeme[0] == "глазки":
        lexeme = [l for l in lexeme if l not in ('глазков', )]

    if lexeme[0] == "портки":
        lexeme = [l for l in lexeme if l not in ('портков', )]

    if lexeme[0] == "суда":
        lexeme = [l for l in lexeme if l not in ('суднам', 'суднами', 'суднах', 'суден')]

    if lexeme[0] == "засилья":
        lexeme = [l for l in lexeme if l not in ('засилиям', 'засилиями', 'засилиях')]

    if lexeme[0] == "барыши":
        lexeme = [l for l in lexeme if l not in ('барышов')]

    if lexeme[0] == "сердца":
        lexeme = [l for l in lexeme if l not in ('сердцев',)]

    if lexeme[0] == "колы":
        lexeme = [l for l in lexeme if not 'коль' in l]

    if lexeme[0] == "суслята":
        lexeme = [l for l in lexeme if not 'суслен' in l]

    if lexeme[0] == "дышла":
        lexeme = [l for l in lexeme if l not in ('дышл',)]

    if lexeme[0] == "дочери":
        lexeme = [l for l in lexeme if l not in ('дочерьми',)]

    if lexeme[0] == "блиночки":
        lexeme = [l for l in lexeme if l not in ('блиночек',)]

    if lexeme[0] == "собратья":
        lexeme = [l for l in lexeme if l not in ('собратий',)]

    if lexeme[0] == "волоконца":
        lexeme = [l for l in lexeme if l not in ('волоконец',)]

    if lexeme[0] == "вывевки":
        lexeme = [l for l in lexeme if l not in ('вывевков',)]

    if lexeme[0] == "звери":
        lexeme = [l for l in lexeme if l not in ('зверьми',)]

    if lexeme[0] == "коленца":
        lexeme = [l for l in lexeme if l not in ('коленцев',)]

    if lexeme[0] == "эрги":
        lexeme = [l for l in lexeme if l not in ('эргов',)]

    if lexeme[0] == "лоскуты":
        lexeme = [l for l in lexeme if l not in ('лоскутьев', 'лоскутьям', 'лоскутьями', 'лоскутьях')]

    if lexeme[0] == "цацки":
        lexeme = [l for l in lexeme if l not in ('цацок', )]

    if lexeme[0] == "оконца":
        lexeme = [l for l in lexeme if l not in ('оконцев', )]

    if lexeme[0] == "дупла":
        lexeme = [l for l in lexeme if l not in ('дупл', )]

    if lexeme[0] == "кости":
        lexeme = [l for l in lexeme if l not in ('костьми', )]

    if lexeme[0] == "санорины":
        lexeme = [l for l in lexeme if l not in ('санориных', 'санориным', 'санориными', 'санориных')]

    if lexeme[0] == "азарины":
        lexeme = [l for l in lexeme if l not in ('азаринов', 'азаринам', 'азаринами', 'азаринах')]

    if lexeme[0] == "листы":
        lexeme = [l for l in lexeme if 'ь' not in l]

    if lexeme[0] == "клоки":
        lexeme = [l for l in lexeme if 'ь' not in l]

    if lexeme[0] == "песни":
        lexeme = [l for l in lexeme if l not in ("песней", )]

    if lexeme[0] == "пруты":
        lexeme = [l for l in lexeme if 'ь' not in l]

    if lexeme[0] == "заем":
        lexeme = [l for l in lexeme if l not in ("займ",)]

    if lexeme[0] == "копытца":
        lexeme = [l for l in lexeme if l not in ("копытцев",)]

    if lexeme[0] == "вороненочки":
        lexeme = [l for l in lexeme if l not in ("вороненочек",)]

    if lexeme[0] == "стрепетята":
        lexeme = [l for l in lexeme if "енк" not in l]

    if lexeme[0] == "крюки":
        lexeme = [l for l in lexeme if "ь" not in l]

    if lexeme[0] == "рожки":
        lexeme = [l for l in lexeme if l not in ("рожков",)]

    if lexeme[0] == "русла":
        lexeme = [l for l in lexeme if l not in ("русл",)]

    if lexeme[0] == "сусла":
        lexeme = [l for l in lexeme if l not in ("сусл",)]

    if lexeme[0] == "стулы":
        lexeme = ["стулья"] + [l for l in lexeme[1:] if "ь" in l]

    if lexeme[0] == "золотца":
        lexeme = [l for l in lexeme if l not in ("золотцев",)]

    if lexeme[0] == "шурины":
        lexeme = [l for l in lexeme if "ь" not in l]

    if lexeme[0] == "шиши":
        lexeme = [l for l in lexeme if l not in ("шишов", )]

    if lexeme[0] == "чибисята":
        lexeme = [l for l in lexeme if "енк" not in l]

    if lexeme[0] == "поленца":
        lexeme = [l for l in lexeme if l not in ("поленцев", )]

    if lexeme[0] == "зубки":
        lexeme = [l for l in lexeme if l not in ("зубков", )]

    if lexeme[0] == "тягла":
        lexeme = [l for l in lexeme if l not in ("тягл", )]

    if lexeme[0] == "парсеки":
        lexeme = [l for l in lexeme if l not in ("парсеков", )]

    if lexeme[0] == "кочаны":
        lexeme = [l for l in lexeme if "чан" in l]

    if lexeme[0] == "мегаэрги":
        lexeme = [l for l in lexeme if l not in ("мегаэргов", )]

    # братий
    if lexeme[0] == "братья":
        lexeme = [l for l in lexeme if 'ь' in l]

    if lexeme[0] == "лошади":
        lexeme = [l for l in lexeme if 'ь' not in l]

    if lexeme[0] == "зубы":
        lexeme = [l for l in lexeme if 'ь' not in l]

    if lexeme[0] == "суконца":
        lexeme = [l for l in lexeme if l not in ("суконцев", )]

    if lexeme[0] == "колена":
        lexeme = [l for l in lexeme if 'ь' not in l and l not in ('колен', 'коленям', 'коленями', 'коленях')]

    if lexeme[0] == "шубы":
        lexeme = [l for l in lexeme if l not in ("шубых", "шубым", "шубыми")]

    if lexeme[0] == "электрополотенца":
        lexeme = [l for l in lexeme if l not in ("электрополотенецев")]

    if lexeme[0] == "корытца":
        lexeme = [l for l in lexeme if l not in ("корытцев", )]

    if lexeme[0] == "шлицы":
        lexeme = [l for l in lexeme if l not in ("шлицев", )]

    if lexeme[0] == "очески":
        lexeme = [l for l in lexeme if l not in ("оческов", )]

    if lexeme[0] == "крыла":
        lexeme = ["крылья"] + [l for l in lexeme if "ь" in l]

    if lexeme[0] == "кон" and only_mult:
        lexeme = ["коны"] + lexeme[1:]

    if lexeme[0] == "деревца":
        lexeme = [l for l in lexeme if l not in ("деревцев", )]

    if lexeme[0] == "электрорубанки":
        lexeme = [l for l in lexeme if l not in ("электрорубанок", )]

    if lexeme[0] == "цветки":
        lexeme = [l for l in lexeme if 'к' in l]

    if lexeme[0] == "курицы":
        lexeme = [l for l in lexeme if "иц" in l]

    if lexeme[0] == "рис" and "рисы" in lexeme:
        lexeme = ["рисы"] + lexeme[1:]

    if lexeme[0] == "сыновья":
        lexeme = [l for l in lexeme if "ь" in l or l in ("сыновей",)]

    if lexeme[0] == "опенки":
        lexeme = [l for l in lexeme if 'н' in l]

    if lexeme[0] == "полотенца":
        lexeme = [l for l in lexeme if l not in ("полотенцев", )]

    if lexeme[0] == "барвиночки":
        lexeme = [l for l in lexeme if l not in ("барвиночек", )]

    if lexeme[0] == "мужья":
        lexeme = [l for l in lexeme if l not in ("мужам", "мужами", "мужах")]

    if lexeme[0] == "поленья":
        lexeme = [l for l in lexeme if l not in ("полен", )]

    if lexeme[0] == "кружевца":
        lexeme = [l for l in lexeme if l not in ("кружевцев", )]

    if lexeme[0] == "долотца":
        lexeme = [l for l in lexeme if l not in ("долотцев", )]

    if lexeme[0] == "суки":
        lexeme = [l for l in lexeme if "ь" not in l]

    if lexeme[0] == "ягуарята":
        lexeme = [l for l in lexeme if "енк" not in l]

    if lexeme[0] == "турята":
        lexeme = [l for l in lexeme if "енк" not in l]

    if lexeme[0] == "ушки":
        lexeme = [l for l in lexeme if l not in ("ушков", )]

    if lexeme[0] == "надкрылья":
        lexeme = [l for l in lexeme if "ь" in l]

    if lexeme[0] == "комели":
        lexeme = [l for l in lexeme if "мел" in l]

    if lexeme[0] == "решетца":
        lexeme = [l for l in lexeme if l not in ("решетцев", )]

    if lexeme[0] == "струпья":
        lexeme = [l for l in lexeme if l not in ("струпов", )]

    if lexeme[0] == "чуда":
        lexeme = [l for l in lexeme if "с" not in l]

    if lexeme[0] == "вытопки":
        lexeme = [l for l in lexeme if l not in ("вытопков", )]

    if lexeme[0] == "грабли":
        lexeme = [l for l in lexeme if l not in ("грабель", )]

    if lexeme[0] == "северянины":
        lexeme = [l for l in lexeme if "и" in l and l != "северянами"]

    if lexeme[0] == "князи":
        lexeme = ["князья"] + lexeme[1:]

    if lexeme[0] == "ситца":
        lexeme = [l for l in lexeme if l not in ("ситцев", )]

    if lexeme[0] == "веретенца":
        lexeme = [l for l in lexeme if l not in ("веретенцев", )]

    if lexeme[0] == "медвежоночки":
        lexeme = [l for l in lexeme if l not in ("медвежоночек", )]

    if lexeme[0] == "порточки":
        lexeme = [l for l in lexeme if l not in ("порточков", )]

    if lexeme[0] == "неба":
        lexeme = [l for l in lexeme if "с" not in l]

    if lexeme[0] == "ножны":
        lexeme = [l for l in lexeme if l not in ("ножон", )]

    if lexeme[0] == "отсевки":
        lexeme = [l for l in lexeme if l not in ("отсевков", )]

    if lexeme[0] == "рубины":
        lexeme = [l for l in lexeme if l not in ("рубиных", "рубиными", "рубиных")]

    if lexeme[0] == "осевки":
        lexeme = [l for l in lexeme if l not in ("осевков", )]

    if lexeme[0] == "щенки":
        lexeme = [l for l in lexeme if "ят" not in l]

    if lexeme[0] == "кий":
        lexeme = [l for l in lexeme if l not in ("кие", )]

    if lexeme[0] == "пригоршни":
        lexeme = [l for l in lexeme if l not in ("пригоршен", )]

    if lexeme[0] == "плечи":
        lexeme = [l for l in lexeme if l not in ("плечьми", "плеч")]

    if lexeme[0] == "веча":
        lexeme = [l for l in lexeme if l not in ("веч", )]

    if lexeme[0] == "ведерца":
        lexeme = [l for l in lexeme if l not in ("ведерец", )]

    if lexeme[0] == "хохлач":
        lexeme = [l for l in lexeme if l not in ("хохлачем", )]

    if lexeme[0] == "молодец":
        lexeme = [l for l in lexeme if l not in ("молодцем", )]

    if lexeme[0] == "молодцы":
        lexeme = [l for l in lexeme if l not in ("молодцев", )]

    if lexeme[0] == "смехотворцы":
        lexeme = [l for l in lexeme if l not in ("смехотворцов", )]

    if lexeme[0] == "силикомарганцы":
        lexeme = [l for l in lexeme if l not in ("силикомарганцов", )]

    if lexeme[0] == "военспецы":
        lexeme = [l for l in lexeme if l not in ("военспецев", )]

    if lexeme[0] == "венцы":
        lexeme = [l for l in lexeme if l not in ("венцев", )]

    if lexeme[0] == "воронцы":
        lexeme = [l for l in lexeme if l not in ("воронцев", )]

    if lexeme[0] == "чесанцы":
        lexeme = [l for l in lexeme if l not in ("чесанцев", )]

    if lexeme[0] == "спецы":
        lexeme = [l for l in lexeme if l not in ("спецев", )]

    if lexeme[0] == "мальцы":
        lexeme = [l for l in lexeme if l not in ("мальцев", )]

    if lexeme[0] == "аракчеевцы":
        lexeme = [l for l in lexeme if l not in ("аракчеевцов", )]

    if lexeme[0] == "праотцы":
        lexeme = [l for l in lexeme if l not in ("праотцев", )]

    if lexeme[0] == "жук-плавунцы":
        lexeme = [l for l in lexeme if l not in ("жук-плавунцев", )]

    if lexeme[0] == "отец":
        lexeme = [l for l in lexeme if l not in ("отче", )]

    if lemma.endswith("ишко") and only_single:
        for i in range(len(lexeme)):
            if lexeme[i].endswith("ой"):
                lexeme[i] = lexeme[i].replace("ой", "ом")
        lexeme = [l for l in lexeme if not l.endswith("и")]

    if lexeme[0].endswith("ие") and lemma.endswith("ие"):
        lexeme = [l for l in lexeme if not 'ь' in l[-3:]]

    if lexeme[0].endswith("ия") and lemma.endswith("ие"):
        lexeme = [l for l in lexeme if not 'ь' in l[-4:-1]]

    if lexeme[0].endswith("ье") and lemma.endswith("ье"):
        lexeme = [l for l in lexeme if not 'и' in l[-3:]]

    if lexeme[0] not in ("рукобитья", "засилья", "челобитья", "зазимья", "затишья", "первозимья", "предзимья"):
        if lexeme[0].endswith("ья") and lemma.endswith("ье"):
            lexeme = [l for l in lexeme if not 'и' in l[-4:-1]]

    if all(l == lexeme[0] for l in lexeme):
        return [lexeme[0]]

    # TODO fixme (note)
    # ['папайи', 'папай', 'папайям', 'папайи', 'папайями', 'папайях']
    if lexeme[0] == "секвойи" or lexeme[0] == "паранойи" or lexeme[0] == "папайи":
        return []

    # TODO very complex lexeme
    if lexeme[0] == "любови":
        return []

    # garbage
    if lexeme[0] == "подкрылье" or lexeme[0] == "староселье":
        return []

    if lexeme[0] == "развальцы":
        return []

    # TODO fixme
    #['мглы', 'мглам', 'мглы', 'мглами', 'мглах']
    if lexeme[0] == "мзды" or lexeme[0] == "мглы" or lexeme[0] == "полумглы" or lexeme[0] == "полутьмы" or lexeme[0] == "хны":
        return []

    # ['года', 'годов', 'лет', 'годам', 'года', 'годами', 'годах', 'гг', 'гг', 'гг', 'гг', 'летам', 'летами', 'летах']
    # TODO FIX
    if lexeme[0] == "года":
        return []

    # garbage
    if lexeme[0] == "сполье":
        return []

    # broken lexeme
    if lexeme[0] == "одним-одины":
        return []

    if lemma == "полминуты":
        return []

    # something wrong with lexeme
    if lemma == "полмира":
        return []

    if lexeme[0] == "полдни":
        return []

    if lemma == "полмиллиона":
        return []

    # broken lexeme
    if lemma == "кабельтов":
        return []

    # broken lexeme
    if lexeme[0] == "антропос":
        return []

    if lemma == "пики":
        return []

    # incorrect lexeme
    if lexeme[0].endswith("-сырец"):
        return []

    # TODO remove
    if lexeme[0] == "корчмы" or lexeme[0] == "кошмы":
        return []

    # TODO remove
    if lexeme[0] == "нижеподписавшиеся" or lexeme[0] == "вольноопределяющиеся":
        return []

    # TODO fix
    if lemma in ('счет-фактура', 'город-спутник', 'ванька-встанька', 'школа-интернат', 'комедия-буфф', 'дед-мороз',
                     'шар-зонд', 'город-герой', 'ракета-носитель', 'вагон-ресторан', 'сыр-бор', 'баба-яга',
                     'счет-фактура', 'город-спутник', 'ванька-встанька', 'член-корреспондент'):
        return []

    first_six = lexeme[:6]
    result = first_six[:]
    for l in lexeme[6:]:
        if l not in first_six:
            result.append(l)

    return result
