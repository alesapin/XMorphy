#!/usr/bin/env python3

def preprocess_adj_lexeme(lexeme):
    if all(l == lexeme[0] for l in lexeme):
        return [lexeme[0]]

    if lexeme[0] == "княжой":
        lexeme.append("княжие")
        lexeme.append("княжих")
        lexeme.append("княжими")

    if lexeme[0] == "горько-сладкий" or lexeme[0] == "солено-сладкий" or lexeme[0] == "приторно-сладкий" or lexeme[0] == "пестро-яркий":
        return []

    if lexeme[0] == "полунезависимый" or lexeme[0] == "полупочтенный":
        return []
    # ADV
    if lexeme[0] in ("невиданнее", "немыслимее", "повзводнее", "поэтапнее", "подекаднее", "наполнее", "подобнее", "позднее", "поздноватее", "наплотнее") or len(lexeme) <= 3:
        return []

    if lexeme[0] == "прекрутой":
        return []

    # something wrong
    if lexeme[0] == "полугарный" or lexeme[0] == "заорганизованный" or lexeme[0] == "полупомешанный":
        return []

    if lexeme[0] == "прежестокий":
        return [l for l in lexeme if "айш" not in l and l not in ('прежесточе', 'препожесточе')]

    if lexeme[0] == "перепеленатый":
        return [l for l in lexeme if "нут" not in l]

    if lexeme[0] == "воздвигнувшийся":
        return [l for l in lexeme if "ну" in l]

    if lexeme[0] == "полоскающийся":
        return [l for l in lexeme if "полощу" not in l]

    if lexeme[0] == "богатый" or lexeme[0] == "полустойкий" or lexeme[0] == "хлябкий":
        return [l for l in lexeme if "ейш" not in l and "че" not in l]

    if lexeme[0] == "мурлыкающий":
        return [l for l in lexeme if "ч" not in l]

    if lexeme[0] == "предолгий":
        return [l for l in lexeme if "ольш" not in l]

    if lexeme[0] == "отторгнувшийся":
        return [l for l in lexeme if "ну" in l]

    if lexeme[0] == "полоскающий":
        return [l for l in lexeme if "полощу" not in l]

    if lexeme[0] == "перегаснувший":
        return [l for l in lexeme if "ну" in l]

    if lexeme[0] == "курлыкающий":
        return [l for l in lexeme if "чу" not in l]

    if lexeme[0] == "архисовременный":
        lexeme = [l for l in lexeme if "ейш" not in l]

    if lexeme[0] == "спеленатый":
        lexeme = [l for l in lexeme if "ну" not in l]

    if lexeme[0] == "погаснувший":
        lexeme = [l for l in lexeme if "ну" in l]

    if lexeme[0] == "разлогий":
        lexeme = [l for l in lexeme if "же" not in l]

    if lexeme[0] == "исторгнувшийся":
        lexeme = [l for l in lexeme if "ну" in l]

    if lexeme[0] == "подрогнувший":
        lexeme = [l for l in lexeme if "ну" in l]

    if lexeme[0] == "квазиупругий":
        lexeme = [l for l in lexeme if "же" not in l]

    if lexeme[0] == "экстрамодный" or lexeme[0] == "полупочтенный" or lexeme[0] == "квазиактивный" or lexeme[0] == "прекрасивый" or lexeme[0] == "полумягкий":
        lexeme = [l for l in lexeme if "наи" not in l and "ейш" not in l and not "ше" in l and not "айш" in l and "гче" not in l]

    if lexeme[0] == "постигнувший" or lexeme[0] == "низвергнувшийся":
        lexeme = [l for l in lexeme if "ну" in l]

    if lexeme[0] == "преглухой":
        lexeme = [l for l in lexeme if "ше" not in l]

    if lexeme[0] == "плескающийся":
        lexeme = [l for l in lexeme if "щущ" not in l]

    if lexeme[0] == "извергнувшийся":
        lexeme = [l for l in lexeme if "ну" in l]

    if lexeme[0] == "проникнувшийся":
        lexeme = [l for l in lexeme if "ну" in l]

    if lexeme[0] == "упеленатый":
        lexeme = [l for l in lexeme if "ну" not in l]

    if lexeme[0] == "воскреснувший" or lexeme[0] == "ниспровергнувший":
        lexeme = [l for l in lexeme if "ну" in l]

    if lexeme[0] == "исторгнувший" or lexeme[0] == "ввязнувший":
        lexeme = [l for l in lexeme if "ну" in l]

    if lexeme[0] == "паркий":
        lexeme = [l for l in lexeme if "че" not in l]

    if lexeme[0] == "запеленатый" or  lexeme[0] == "распеленатый":
        lexeme = [l for l in lexeme if "ну" not in l]

    if lexeme[0] == "иссякнувший" or lexeme[0] == "свергнувшийся" or lexeme[0] == "передрогнувший" or lexeme[0] == "засохнувший":
        lexeme = [l for l in lexeme if "ну" in l]

    if lexeme[0] == "копоткий" or lexeme[0] == "варкий" or lexeme[0] == "солкий":
        lexeme = [l for l in lexeme if "че" not in l]

    if lexeme[0] == "плескающий":
        lexeme = [l for l in lexeme if "щущ" not in l]

    if lexeme[0] == "дорогой":
        lexeme = [l for l in lexeme if "же" not in l]

    if lexeme[0] == "щепающийся":
        lexeme = [l for l in lexeme if "л" not in l]

    if lexeme[0] == "претерпенный":
        lexeme = [l for l in lexeme if "л" not in l]

    if lexeme[0].startswith("па") and not lexeme[0].startswith("папо"):
        lexeme = [l for l in lexeme if not l.startswith("папо")]

    if lexeme[0].startswith("об") and not lexeme[0].startswith("обпо"):
        lexeme = [l for l in lexeme if not l.startswith("обпо")]

    if lexeme[0].startswith("авиа") and not lexeme[0].startswith("авиапо"):
        lexeme = [l for l in lexeme if not l.startswith("авиапо")]

    if lexeme[0].startswith("им") and not lexeme[0].startswith("импо"):
        lexeme = [l for l in lexeme if not l.startswith("импо")]

    if lexeme[0].startswith("меж") and not lexeme[0].startswith("межпо"):
        lexeme = [l for l in lexeme if not l.startswith("межпо")]

    if lexeme[0].startswith("еже") and not lexeme[0].startswith("ежепо"):
        lexeme = [l for l in lexeme if not l.startswith("ежепо")]

    if lexeme[0].startswith("пере") and not lexeme[0].startswith("перепо"):
        lexeme = [l for l in lexeme if not l.startswith("перепо")]

    if lexeme[0].startswith("предъ") and not lexeme[0].startswith("предъпо"):
        lexeme = [l for l in lexeme if not l.startswith("предъпо")]

    if lexeme[0].startswith("интра") and not lexeme[0].startswith("интрапо"):
        lexeme = [l for l in lexeme if not l.startswith("интрапо")]

    if lexeme[0].startswith("ультрапо") and not lexeme[0].startswith("ультрапопо"):
        lexeme = [l for l in lexeme if not l.startswith("ультрапопо")]

    if lexeme[0].startswith("разъ") and not lexeme[0].startswith("разъпо"):
        lexeme = [l for l in lexeme if not l.startswith("разъпо")]

    if lexeme[0].startswith("с") and not lexeme[0].startswith("спо"):
        lexeme = [l for l in lexeme if not l.startswith("спо")]

    if lexeme[0].startswith("попо") and not lexeme[0].startswith("попопо"):
        lexeme = [l for l in lexeme if not l.startswith("попопо")]

    if lexeme[0].startswith("к") and not lexeme[0].startswith("кпо"):
        lexeme = [l for l in lexeme if not l.startswith("кпо")]

    if lexeme[0].startswith("экстра") and not lexeme[0].startswith("экстрапо"):
        lexeme = [l for l in lexeme if not l.startswith("экстрапо")]

    if lexeme[0].startswith("сверх") and not lexeme[0].startswith("сверхпо"):
        lexeme = [l for l in lexeme if not l.startswith("сверхпо")]

    if lexeme[0].startswith("со") and not lexeme[0].startswith("сопо"):
        lexeme = [l for l in lexeme if not l.startswith("сопо")]

    if lexeme[0].startswith("постъ") and not lexeme[0].startswith("постъпо"):
        lexeme = [l for l in lexeme if not l.startswith("постъпо")]

    if lexeme[0].startswith("псевдо") and not lexeme[0].startswith("псевдопо"):
        lexeme = [l for l in lexeme if not l.startswith("псевдопо")]

    if lexeme[0].startswith("контр") and not lexeme[0].startswith("контрпо"):
        lexeme = [l for l in lexeme if not l.startswith("контрпо")]

    if lexeme[0].startswith("квази") and not lexeme[0].startswith("квазипо"):
        lexeme = [l for l in lexeme if not l.startswith("квазипо")]

    if lexeme[0].startswith("вы") and not lexeme[0].startswith("выпо"):
        lexeme = [l for l in lexeme if not l.startswith("выпо")]

    if lexeme[0].startswith("при") and not lexeme[0].startswith("припо"):
        lexeme = [l for l in lexeme if not l.startswith("припо")]

    if lexeme[0].startswith("а") and not lexeme[0].startswith("апо"):
        lexeme = [l for l in lexeme if not l.startswith("апо")]

    if lexeme[0].startswith("межъ") and not lexeme[0].startswith("межъпо"):
        lexeme = [l for l in lexeme if not l.startswith("межъпо")]

    if lexeme[0].startswith("на") and not lexeme[0].startswith("напо"):
        lexeme = [l for l in lexeme if not l.startswith("напо")]

    if lexeme[0].startswith("запо") and not lexeme[0].startswith("запопо"):
        lexeme = [l for l in lexeme if not l.startswith("запопо")]

    if lexeme[0].startswith("до") and not lexeme[0].startswith("допо"):
        lexeme = [l for l in lexeme if not l.startswith("допо")]

    if lexeme[0].startswith("пред") and not lexeme[0].startswith("предпо"):
        lexeme = [l for l in lexeme if not l.startswith("предпо")]

    if lexeme[0].startswith("рас") and not lexeme[0].startswith("распо"):
        lexeme = [l for l in lexeme if not l.startswith("распо")]

    if lexeme[0].startswith("препо") and not lexeme[0].startswith("препопо"):
        lexeme = [l for l in lexeme if not l.startswith("препопо")]

    if lexeme[0].startswith("под") and not lexeme[0].startswith("подпо"):
        lexeme = [l for l in lexeme if not l.startswith("подпо")]

    if lexeme[0].startswith("анти") and not lexeme[0].startswith("антипо"):
        lexeme = [l for l in lexeme if not l.startswith("антипо")]

    if lexeme[0].startswith("пре") and not lexeme[0].startswith("пренаи"):
        lexeme = [l for l in lexeme if not l.startswith("пренаи")]

    if lexeme[0].startswith("после") and not lexeme[0].startswith("послепо"):
        lexeme = [l for l in lexeme if not l.startswith("послепо")]

    if lexeme[0].startswith("ре") and not lexeme[0].startswith("репо"):
        lexeme = [l for l in lexeme if not l.startswith("репо")]

    if lexeme[0].startswith("архи") and not lexeme[0].startswith("архипо"):
        lexeme = [l for l in lexeme if not l.startswith("архипо")]

    if lexeme[0].startswith("полу") and not lexeme[0].startswith("полупо"):
        lexeme = [l for l in lexeme if not l.startswith("полупо")]

    if lexeme[0].startswith("беспо") and not lexeme[0].startswith("беспопо"):
        lexeme = [l for l in lexeme if not l.startswith("беспопо")]

    if lexeme[0].startswith("бес") and not lexeme[0].startswith("беспо"):
        lexeme = [l for l in lexeme if not l.startswith("беспо")]

    if lexeme[0].startswith("без") and not lexeme[0].startswith("безпо"):
        lexeme = [l for l in lexeme if not l.startswith("безпо")]

    if lexeme[0].startswith("безъ") and not lexeme[0].startswith("безъпо"):
        lexeme = [l for l in lexeme if not l.startswith("безъпо")]

    if lexeme[0].startswith("пре") and not lexeme[0].startswith("препо"):
        lexeme = [l for l in lexeme if not l.startswith("препо")]

    if lexeme[0].startswith("по") and not lexeme[0].startswith("попо"):
        lexeme = [l for l in lexeme if not l.startswith("попо")]

    if lexeme[0].startswith("вне") and not lexeme[0].startswith("внепо"):
        lexeme = [l for l in lexeme if not l.startswith("внепо")]

    return lexeme
