from pyxmorphy import MorphAnalyzer, PhemTag


def _print_tag(tag):
    if tag == PhemTag.B_SUFF:
        return str(PhemTag.SUFF)
    if tag == PhemTag.B_ROOT:
        return str(PhemTag.ROOT)
    if tag == PhemTag.B_PREF:
        return str(PhemTag.PREF)

    return str(tag)


def _tags_equal(left, right):
    if left == PhemTag.B_PREF and right == PhemTag.B_PREF:
        return False
    if left == PhemTag.B_ROOT and right == PhemTag.B_ROOT:
        return False
    if left == PhemTag.B_SUFF and right == PhemTag.B_SUFF:
        return False

    if left == PhemTag.B_PREF and right == PhemTag.PREF:
        return True
    if left == PhemTag.B_ROOT and right == PhemTag.ROOT:
        return True
    if left == PhemTag.B_SUFF and right == PhemTag.SUFF:
        return True

    return left == right


def phem_info_to_string(wf):
    word = wf.word_form
    phem_info = wf.phem_info

    if not phem_info or not word:
        return ""

    result = word[0]
    prev = phem_info[0]
    for i in range(1, len(word)):
        if not _tags_equal(prev, phem_info[i]):
            result += ":" + _print_tag(prev) + "/"
        prev = phem_info[i]
        result += word[i]

    result += ":" + _print_tag(prev)

    return result


def read_corpus(corpus_stream):
    result = []
    i = 0
    for line in corpus_stream:
        i += 1
        line_arr = line.strip().split('\t')
        if not line_arr:
            continue
        result.append((line_arr[0], line_arr[1]))
        if i % 1000 == 0:
            print("Parsed:", i)

    return result


def test_accuracy(morphem_file):
    analyzer = MorphAnalyzer()
    total_words = 0
    total_correct = 0
    with open(morphem_file, 'r') as f:
        words = read_corpus(f)
        error_by_sp = {}
        for word, orig_parse in words:
            total_words += 1
            parsed_word = analyzer.analyze_single_word(word, True, True)
            test_parse = phem_info_to_string(parsed_word)
            if test_parse == orig_parse:
                total_correct += 1
            else:
                sp = str(parsed_word.infos[0].sp)
                if sp not in error_by_sp:
                    error_by_sp[sp] = 0
                error_by_sp[sp] += 1
            if total_words % 1000 == 0:
                print("Processed:", total_words, "correct", total_correct / total_words)
            if total_words > 30000:
                break

    print("Total:", total_words)
    print("Correct words:", total_correct)
    print("Ratio:", total_correct / total_words)
    print("Error by sp:", error_by_sp)
    assert total_correct / total_words > 0.87
