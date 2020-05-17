import pyxmorphy
import string


class WordInCorpus(object):
    def __init__(self, word_form, normal_form, sp, tag):
        self.word_form = word_form
        self.normal_form = normal_form
        self.sp = sp
        self.tag = tag


PUNCTS = ''.join([p for p in string.punctuation if p != '-' and p != '_'])


class Sentence(object):
    def __init__(self):
        self.words = []

    def _fix_wf(self, wf):
        if len(wf) == 1 or wf == '...':
            return wf
        return wf.translate(str.maketrans('', '', PUNCTS))

    def add_word_from_line(self, line_arr):
        wf = self._fix_wf(line_arr[1])
        nf = self._fix_wf(line_arr[2].replace('ё', 'е').replace('Ё', 'е').lower())
        sp = line_arr[3]
        tag = line_arr[4]
        self.words.append(WordInCorpus(wf, nf, sp, tag))

    def as_string(self):
        return ' '.join([w.word_form for w in self.words])

    def __len__(self):
        return len(self.words)


def read_corpus_sentence_by_sentece(corpus_stream):
    result = []
    sentence = Sentence()
    i = 0
    for line in corpus_stream:
        i += 1
        line_arr = line.strip().split('\t')
        if (not line_arr or not line_arr[0]) and len(sentence) > 0:
            result.append(sentence)
            sentence = Sentence()
        else:
            sentence.add_word_from_line(line_arr)
        if i % 20000 == 0:
            print("Processing line", i)

    result.append(sentence)
    return result


class Stats(object):
    def __init__(self, total, nf, sp, case, gender, number, tense):
        self.total = total
        self.nf = nf
        self.sp = sp
        self.case = case
        self.gender = gender
        self.number = number
        self.tense = tense


def check_tag(tag, tag_line, tagname):
    if str(tag) in tag_line:
        return True
    if str(tag) == '_' and tagname not in tag_line:
        return True
    return False


def process_single_sentence(sentence, analyzer):
    analyzed = analyzer.analyze(sentence.as_string(), True, True, False)
    analyzed_sentence = [wf for wf in analyzed if wf.token_type != pyxmorphy.TokenTypeTag.SEPR]
    nf_correct = 0
    sp_correct = 0
    case_correct = 0
    gender_correct = 0
    number_correct = 0
    tense_correct = 0
    total_words = 0
    for analyzed, real in zip(analyzed_sentence, sentence.words):
        if real.sp == str(pyxmorphy.UniSPTag.PUNCT):
            sp_correct += 1
            nf_correct += 1
            case_correct += 1
            gender_correct += 1
            number_correct += 1
            tense_correct += 1
            total_words += 1
            continue

        # some strange words in corpus
        if analyzed.word_form != real.word_form:
            continue

        total_words += 1
        most_probable = analyzed.infos[0]
        if most_probable.normal_form == real.normal_form:
            nf_correct += 1
        if str(most_probable.sp) == str(real.sp):
            sp_correct += 1

        case_correct += check_tag(most_probable.tag.get_case(), real.tag, 'Case')
        gender_correct += check_tag(most_probable.tag.get_gender(), real.tag, 'Gender')
        number_correct += check_tag(most_probable.tag.get_number(), real.tag, 'Number')
        tense_correct += check_tag(most_probable.tag.get_tense(), real.tag, 'Tense')

    return Stats(
        total_words,
        nf_correct,
        sp_correct,
        case_correct,
        gender_correct,
        number_correct,
        tense_correct)


def test_accuracy(corpus_file):
    total_words = 0
    total_nf_correct = 0
    total_sp_correct = 0
    total_case_correct = 0
    total_gender_correct = 0
    total_number_correct = 0
    total_tense_correct = 0

    with open(corpus_file, 'r') as f:
        sentences = read_corpus_sentence_by_sentece(f)

    print("Total sentences:", len(sentences))
    analyzer = pyxmorphy.MorphAnalyzer()
    i = 0
    for sentence in sentences:
        i += 1
        stats = process_single_sentence(sentence, analyzer)
        total_words += stats.total
        if i % 5000 == 0:
            print("Processing sentence", i, "total words", total_words)

        total_nf_correct += stats.nf
        total_sp_correct += stats.sp
        total_case_correct += stats.case
        total_gender_correct += stats.gender
        total_number_correct += stats.number
        total_tense_correct += stats.tense

    print("Total:", total_words)
    print("SP Correct", total_sp_correct / total_words)
    print("NF Correct", total_nf_correct / total_words)
    print("Case Correct", total_case_correct / total_words)
    print("Gender Correct", total_gender_correct / total_words)
    print("Number Correct", total_number_correct / total_words)
    print("Tense Correct", total_tense_correct / total_words)
    assert total_sp_correct / total_words > 0.95
    assert total_nf_correct / total_words > 0.92
    assert total_case_correct / total_words > 0.96
    assert total_gender_correct / total_words > 0.955
    assert total_number_correct / total_words > 0.97
    assert total_tense_correct / total_words > 0.99

