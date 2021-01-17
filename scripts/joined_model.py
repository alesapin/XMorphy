from tensorflow.keras.models import Sequential, Model
from tensorflow.keras.layers import LSTM, Bidirectional, Conv1D, Flatten, Lambda, RepeatVector
from tensorflow.keras.layers import Dense, Input, Concatenate, Masking, MaxPooling1D
from tensorflow.keras.layers import TimeDistributed, Dropout, BatchNormalization, Activation
from tensorflow.keras.utils import to_categorical
from tensorflow.keras.optimizers import Adam
import numpy as np

from keras.preprocessing.sequence import pad_sequences
from tensorflow.keras.callbacks import EarlyStopping, LearningRateScheduler
import time
import pyxmorphy
from pyxmorphy import UniSPTag, UniMorphTag
import fasttext
from enum import Enum

SPEECH_PARTS = [
    'X',
    'ADJ',
    'ADV',
    'INTJ',
    'NOUN',
    'PROPN',
    'VERB',
    'ADP',
    'AUX',
    'CONJ',
    'SCONJ',
    'DET',
    'NUM',
    'PART',
    'PRON',
    'PUNCT',
    'H',
    'R',
    'Q',
    'SYM',
]

SPEECH_PART_MAPPING = {str(s): num for num, s in enumerate(SPEECH_PARTS)}

MASK_VALUE = 0.0


def build_speech_part_array(sp):
    output = [0. for _ in range(len(SPEECH_PARTS))]
    output[SPEECH_PART_MAPPING[str(sp)]] = 1.
    return output


PARTS_MAPPING = {
    'UNKN': 0,
    'PREF': 1,
    'ROOT': 2,
    'SUFF': 3,
    'END': 4,
    'LINK': 5,
    'HYPH': 6,
    'POSTFIX': 7,
    'B-SUFF': 8,
    'B-PREF': 9,
    'B-ROOT': 10,
    'NUMB': 11,
}

LETTERS = {
    'о': 1,
    'е': 2,
    'а': 3,
    'и': 4,
    'н': 5,
    'т': 6,
    'с': 7,
    'р': 8,
    'в': 9,
    'л': 10,
    'к': 11,
    'м': 12,
    'д': 13,
    'п': 14,
    'у': 15,
    'я': 16,
    'ы': 17,
    'ь': 18,
    'г': 19,
    'з': 20,
    'б': 21,
    'ч': 22,
    'й': 23,
    'х': 24,
    'ж': 25,
    'ш': 26,
    'ю': 27,
    'ц': 28,
    'щ': 29,
    'э': 30,
    'ф': 31,
    'ъ': 32,
    'ё': 33,
    '-': 34,
}


VOWELS = {
    'а', 'и', 'е', 'ё', 'о', 'у', 'ы', 'э', 'ю', 'я'
}


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
        return len([morpheme for morpheme in self.morphemes
                    if morpheme.label == MorphemeLabel.SUFFIX])

    def get_speech_part(self):
        return self.sp

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


def parse_word(wordform, parse, sp):
    if ':' in wordform or '/' in wordform:
        return Word([Morpheme(wordform, MorphemeLabel['UNKN'], 0)], sp)

    parts = parse.split('/')
    morphemes = []
    global_index = 0
    for part in parts:
        morphemes.append(parse_morpheme(part, global_index))
        global_index += len(part)
    return Word(morphemes, sp)


def measure_quality(predicted_targets, targets, words, verbose=False):
    TP, FP, FN, equal, total = 0, 0, 0, 0, 0
    SE = ['{}-{}'.format(x, y) for x in "SE" for y in ["ROOT", "PREF", "SUFF", "END", "LINK", "UNKN", "HYPH", "NUMB"]]
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
        equal += sum(int(x == y) for x, y in zip(corr, pred))
        total += len(corr)
        corr_words += (corr == pred)
        if corr != pred and verbose:
            print("Error in word '{}':\n correct:".format(word), corr, '\n!=\n wrong:', pred)

    metrics = ["Precision", "Recall", "F1", "Accuracy", "Word accuracy"]
    results = [TP / (TP+FP), TP / (TP+FN), TP / (TP + 0.5*(FP+FN)),
               equal / total, corr_words / len(targets)]
    return list(zip(metrics, results))

def _transform_classification(parse):
    parts = []
    current_part = [parse[0]]
    for num, letter in enumerate(parse[1:]):
        index = num + 1
        if letter == 'SUFF' and parse[index - 1] == 'B-SUFF':
            current_part.append(letter)
        elif letter == 'PREF' and parse[index - 1] == 'B-PREF':
            current_part.append(letter)
        elif letter == 'ROOT' and parse[index - 1] == 'B-ROOT':
            current_part.append(letter)
        elif letter != parse[index - 1] or letter.startswith('B-'):
            parts.append(current_part)
            current_part = [letter]
        else:
            current_part.append(letter)
    if current_part:
        parts.append(current_part)

    for part in parts:
        if part[0] == 'B-PREF':
            part[0] = 'PREF'
        if part[0] == 'B-SUFF':
            part[0] = 'SUFF'
        if part[0] == 'B-ROOT':
            part[0] = 'ROOT'
        if len(part) == 1:
            part[0] = 'S-' + part[0]
        else:
            part[0] = 'B-' + part[0]
            part[-1] = 'E-' + part[-1]
            for num, letter in enumerate(part[1:-1]):
                part[num+1] = 'M-' + letter
    result = []
    for part in parts:
        result += part
    return result

SPEECH_PARTS = [
    UniSPTag.X,
    UniSPTag.ADJ,
    UniSPTag.ADV,
    UniSPTag.INTJ,
    UniSPTag.NOUN,
    UniSPTag.PROPN,
    UniSPTag.VERB,
    UniSPTag.ADP,
    UniSPTag.AUX,
    UniSPTag.CONJ,
    UniSPTag.SCONJ,
    UniSPTag.DET,
    UniSPTag.NUM,
    UniSPTag.PART,
    UniSPTag.PRON,
    UniSPTag.PUNCT,
    UniSPTag.H,
    UniSPTag.R,
    UniSPTag.Q,
    UniSPTag.SYM,
]

EMBED_SIZE = 300

CASE_TAGS = [
    UniMorphTag.UNKN,
    UniMorphTag.Ins,
    UniMorphTag.Acc,
    UniMorphTag.Nom,
    UniMorphTag.Dat,
    UniMorphTag.Gen,
    UniMorphTag.Loc,
    UniMorphTag.Voc,
]

NUMBER_TAGS = [
    UniMorphTag.UNKN,
    UniMorphTag.Sing,
    UniMorphTag.Plur,
]

GENDER_TAGS = [
    UniMorphTag.UNKN,
    UniMorphTag.Masc,
    UniMorphTag.Fem,
    UniMorphTag.Neut,
]

TENSE_TAGS = [
    UniMorphTag.UNKN,
    UniMorphTag.Fut,
    UniMorphTag.Past,
    UniMorphTag.Pres,
    UniMorphTag.Notpast,
]

ANIMACY_TAGS = [
    UniMorphTag.UNKN,
    UniMorphTag.Anim,
    UniMorphTag.Inan,
]

analyzer = pyxmorphy.MorphAnalyzer()
embedder = fasttext.load_model("rnc_post_soviet_and_morphorueval.embedding_{}.bin".format(EMBED_SIZE))
speech_part_len = len(SPEECH_PARTS)
speech_part_mapping = {str(s): num for num, s in enumerate(SPEECH_PARTS)}

cases_len = len(CASE_TAGS)
case_mapping = {str(s): num for num, s in enumerate(CASE_TAGS)}

numbers_len = len(NUMBER_TAGS)
number_mapping = {str(s): num for num, s in enumerate(NUMBER_TAGS)}

gender_len = len(GENDER_TAGS)
gender_mapping = {str(s): num for num, s in enumerate(GENDER_TAGS)}

tense_len = len(TENSE_TAGS)
tense_mapping = {str(s): num for num, s in enumerate(TENSE_TAGS)}

animacy_len = len(ANIMACY_TAGS)
animacy_mapping = {str(s): num for num, s in enumerate(ANIMACY_TAGS)}


def build_speech_part_array(analyzer_results):
    output = [0 for _ in range(speech_part_len)]
    if analyzer_results:
        for result in analyzer_results.infos:
            output[speech_part_mapping[str(result.sp)]] = 1
    return output


def build_case_array(analyzer_results):
    output = [0 for _ in range(cases_len)]
    if analyzer_results:
        for result in analyzer_results.infos:
            output[case_mapping[str(result.tag.get_case())]] = 1
    return output


def build_number_array(analyzer_results):
    output = [0 for _ in range(numbers_len)]
    if analyzer_results:
        for result in analyzer_results.infos:
            output[number_mapping[str(result.tag.get_number())]] = 1
    return output


def build_gender_array(analyzer_results):
    output = [0 for _ in range(gender_len)]
    if analyzer_results:
        for result in analyzer_results.infos:
            output[gender_mapping[str(result.tag.get_gender())]] = 1
    return output


def build_tense_array(analyzer_results):
    output = [0 for _ in range(tense_len)]
    if analyzer_results:
        for result in analyzer_results.infos:
            output[tense_mapping[str(result.tag.get_tense())]] = 1
    return output


def build_animacy_array(analyzer_results):
    output = [0 for _ in range(animacy_len)]
    if analyzer_results:
        for result in analyzer_results.infos:
            animacy = str(result.tag.get_animacy())
            if animacy not in animacy_mapping:
                output[0] = 1
            else:
                output[animacy_mapping[animacy]] = 1
    return output


def prepare_dataset(path, trim):
    result = []
    sentence = []
    with open(path, 'r') as f:
        i = 0
        try:
            for line in f:
                i += 1
                line = line.strip()
                if not line:
                    result += sentence
                    sentence = []
                else:
                    splited = line.split('\t')
                    tags = splited[6].split('|')
                    speech_part = splited[5]
                    word_form = splited[1]
                    morphemic_parse = splited[2]
                    case = '_'
                    number = '_'
                    gender = '_'
                    tense = '_'
                    for tag in tags:
                        if tag.startswith('Case='):
                            case = tag
                        elif tag.startswith('Number='):
                            number = tag
                        elif tag.startswith('Gender='):
                            gender = tag
                        elif tag.startswith('Tense='):
                            tense = tag
                    word = parse_word(word_form, morphemic_parse, speech_part)
                    sentence.append((word, case, number, gender, tense))
                if i % 1000 == 0:
                    print("Readed:", i)
        except Exception as ex:
            print("last i", i, "line '", line, "'")
            print("Splitted length", len(splited))
            raise ex

    return result[:int(len(result) * trim)]

def _pad_sequences(Xs, Ys, max_len):
    newXs = pad_sequences(Xs, padding='post', dtype=np.int8, maxlen=max_len, value=MASK_VALUE)
    newYs = pad_sequences(Ys, padding='post', maxlen=max_len, value=MASK_VALUE)
    return newXs, newYs

def vectorize_dataset(dataset, maxlen):
    train_encoded = []
    target_sp_encoded = []
    target_case_encoded = []
    target_number_encoded = []
    target_gender_encoded = []
    target_tense_encoded = []
    train_morphem = []
    target_morphem = []

    i = 0
    for features in dataset:
        i += 1
        word = features[0]
        analyzer_result = None
        word_text = word.get_word()
        maxlen = max(len(word_text), maxlen)
        if word_text:
            analyzer_result = analyzer.analyze(word_text, False, False, False)[0]
        word_vector = embedder.get_word_vector(word_text)
        speech_part_vector = build_speech_part_array(analyzer_result)
        case_part_vector = build_case_array(analyzer_result)
        number_vector = build_number_array(analyzer_result)
        gender_vector = build_gender_array(analyzer_result)
        tense_vector = build_tense_array(analyzer_result)
        if word.get_speech_part() not in speech_part_mapping:
            continue
        train_encoded.append(list(word_vector) + speech_part_vector + case_part_vector + number_vector + gender_vector + tense_vector)
        target_sp_encoded.append(to_categorical(speech_part_mapping[word.get_speech_part()], num_classes=len(SPEECH_PARTS)).tolist())
        target_case_encoded.append(to_categorical(case_mapping[features[1]], num_classes=len(case_mapping)).tolist())
        target_number_encoded.append(to_categorical(number_mapping[features[2]], num_classes=len(number_mapping)).tolist())
        target_gender_encoded.append(to_categorical(gender_mapping[features[3]], num_classes=len(gender_mapping)).tolist())
        target_tense_encoded.append(to_categorical(tense_mapping[features[4]], num_classes=len(tense_mapping)).tolist())

        features = []
        for index, letter in enumerate(word_text.lower()):
            letter_features = []
            vovelty = 0
            if letter in VOWELS:
                vovelty = 1
            letter_features.append(vovelty)
            if letter in LETTERS:
                letter_code = LETTERS[letter]
            elif letter_code in ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']:
                letter_code = 35
            else:
                letter_code = 0
            letter_features += to_categorical(letter_code, num_classes=len(LETTERS) + 1 + 1).tolist()
            features.append(letter_features)
        train_morphem.append(np.array(features, dtype=np.int8))
        target_morphem.append(np.array([to_categorical(PARTS_MAPPING[label], num_classes=len(PARTS_MAPPING)) for label in word.get_simple_labels()]))
        if i % 1000 == 0:
            print("Vectorized:", i)

    print("Got maxlen", maxlen)
    padded_train_morphem, padded_target_morphem = _pad_sequences(train_morphem, target_morphem, maxlen)

    return np.asarray(train_encoded), np.asarray(target_sp_encoded), np.asarray(target_case_encoded), np.asarray(target_number_encoded), np.asarray(target_gender_encoded), np.asarray(target_tense_encoded), padded_train_morphem, padded_target_morphem

def scheduler(epoch, lr):
    if epoch < 15:
        return 0.001
    else:
        return 0.0001

class JoinedModel(object):
    def __init__(self, models_number, epochs, validation_split):
        self.models_number = models_number
        self.epochs = epochs
        self.activation = "softmax"
        self.optimizer = Adam(learning_rate=0.001)
        self.models = []
        self.validation_split = validation_split

    def _build_model(self, maxlen):
        inp_morph = Input(shape=(EMBED_SIZE + len(SPEECH_PARTS) + len(CASE_TAGS) + len(NUMBER_TAGS) + len(GENDER_TAGS) + len(TENSE_TAGS),))
        inp_morphem = Input(shape=(maxlen, len(LETTERS) + 1 + 1 + 1))
        inputs = [inp_morph, inp_morphem]

        sp_output = Dense(len(SPEECH_PARTS), activation=self.activation, name="speech_part")(inp_morph)
        case_output = Dense(len(CASE_TAGS), activation=self.activation, name="case")(inp_morph)
        number_output = Dense(len(NUMBER_TAGS), activation=self.activation, name="number")(inp_morph)
        gender_output = Dense(len(GENDER_TAGS), activation=self.activation, name="gender")(inp_morph)
        tense_output = Dense(len(TENSE_TAGS), activation=self.activation, name="tense")(inp_morph)

        repeated_sp = RepeatVector(maxlen)(sp_output)
        repeated_case = RepeatVector(maxlen)(case_output)
        repeated_number = RepeatVector(maxlen)(number_output)
        repeated_gender = RepeatVector(maxlen)(gender_output)
        repeated_tense = RepeatVector(maxlen)(tense_output)
        morphem_features = Concatenate(name="morph_concat")([inp_morphem, repeated_sp, repeated_case, repeated_number, repeated_gender, repeated_tense])
        morphem_convolutions = [morphem_features]
        for drop, units, window_size in zip([0.3, 0.2, 0.1], [512, 512, 512], [5, 5, 5]):
            conv = Conv1D(units, window_size, activation='relu', padding="same")(morphem_convolutions[-1])
            pooling = MaxPooling1D(pool_size=3, data_format='channels_first')(conv)
            norm = BatchNormalization()(pooling)
            do = Dropout(drop)(norm)
            morphem_convolutions.append(do)

        morphem_outputs = [TimeDistributed(
            Dense(len(PARTS_MAPPING), activation=self.activation))(morphem_convolutions[-1])]

        #for i in range(maxlen):
        #    def time_distribute(x, i_from_loop=i):
        #        return x[:, i_from_loop, :]
        #    shaper = Lambda(time_distribute, name="shaper" + str(i))(morphem_convolutions[-1])
        #    concated = Concatenate(name="concat" + str(i) + "morphem")([shaper, sp_output])
        #    morphem_outputs.append(Dense(len(PARTS_MAPPING), activation=self.activation)(concated))

        outputs = [sp_output, case_output, number_output, gender_output, tense_output] + morphem_outputs
        print("Append model")
        self.models.append(Model(inputs, outputs=outputs))
        self.models[-1].compile(loss='categorical_crossentropy',
                                optimizer=self.optimizer, metrics=['acc'])
        print(self.models[-1].summary())

    def train(self, words):
        Xs, Y_sp, Y_case, Y_number, Y_gender, Y_tense, train_morphem, target_morphem = vectorize_dataset(words, 0)
        print("Total models number", self.models_number)
        print("Target morphem shape", target_morphem.shape)
        print("Target morphem first ten", target_morphem[0:10])
        for i in range(self.models_number):
            self.maxlen = len(train_morphem[0])
            self._build_model(self.maxlen)
        print("Total models", len(self.models))
        #morpheme_targets = [elem[:, 0, :] for elem in np.hsplit(target_morphem, self.maxlen)]
        morpheme_targets = [target_morphem]
        print("Targets zero shape", morpheme_targets[0].shape)
        print("Targets zero", morpheme_targets[0][0:10])
        #es1 = EarlyStopping(monitor='val_speech_part_acc', patience=10, verbose=1)
        #es2 = EarlyStopping(monitor='val_case_acc', patience=10, verbose=1)
        learning_scheduler = LearningRateScheduler(scheduler)
        for i, model in enumerate(self.models):
            print("Training", i)
            model.fit([Xs, train_morphem], [Y_sp, Y_case, Y_number, Y_gender, Y_tense] + morpheme_targets, epochs=self.epochs, verbose=2,
                      callbacks=[learning_scheduler], validation_split=self.validation_split, batch_size=8192)
            print("Path", "keras_model_joined_em_{}_{}.h5".format(EMBED_SIZE, int(time.time())))
            model.save("keras_model_joined_em_{}_{}.h5".format(EMBED_SIZE, int(time.time())))
            print("Train finished", i)

    def classify(self, words):
        print("Total models:", len(self.models))
        Xs, Y_SP, Y_CASE, Y_NUMBER, Y_GENDER, Y_TENSE, train_morphem, target_morphem = [np.asarray(elem) for elem in vectorize_dataset(words, self.maxlen)]
        predictions = self.models[0].predict([Xs, train_morphem])
        pred_sp, pred_case, pred_number, pred_gender, pred_tense = predictions[0:5]
        pred_class_sp = pred_sp.argmax(axis=-1)
        pred_class_case = pred_case.argmax(axis=-1)
        pred_class_number = pred_number.argmax(axis=-1)
        pred_class_gender = pred_gender.argmax(axis=-1)
        pred_class_tense = pred_tense.argmax(axis=-1)
        Ysps = Y_SP.argmax(axis=-1)
        Ycases = Y_CASE.argmax(axis=-1)
        Ynumbers = Y_NUMBER.argmax(axis=-1)
        Ygenders = Y_GENDER.argmax(axis=-1)
        Ytences = Y_TENSE.argmax(axis=-1)
        print("Predicted Speech parts")
        print(pred_class_sp[:10])
        print("Actually Speech parts")
        print(Ysps[:10])
        print("Predicted Cases")
        print(pred_class_case[:10])
        print("Actually cases")
        print(Ycases[:10])
        print("Predicted Numbers")
        print(pred_class_number[:10])
        print("Actually Numbers")
        print(Ynumbers[:10])
        print("Predicted Genders")
        print(pred_class_gender[:10])
        print("Actually Genders")
        print(Ygenders[:10])
        print("Predicted Tences")
        print(pred_class_tense[:10])
        print("Actually Tences")
        print(Ytences[:10])

        total_error = set([])
        total_words = len(Ysps)

        error_sps = 0
        word_index = 0
        for pred_word, real_word in zip(pred_class_sp, Ysps):
            if pred_word != real_word:
                total_error.add(word_index)
                error_sps += 1
            word_index += 1

        old_errors = len(total_error)
        print("Errors added by SP:", old_errors)
        print("Total words:", total_words)
        print("Error words:", error_sps)
        print("Error rate SPEECH PART:", float(error_sps) / total_words)
        print("Correct rate SPEECH_PART:", float(total_words - error_sps) / total_words)

        error_cases = 0
        word_index = 0
        for pred_word, real_word in zip(pred_class_case, Ycases):
            if pred_word != real_word:
                total_error.add(word_index)
                error_cases += 1
            word_index += 1

        print("Erros added by case:", len(total_error) - old_errors)
        old_errors = len(total_error)
        print("Total words:", total_words)
        print("Error words:", error_cases)
        print("Error rate Case:", float(error_cases) / total_words)
        print("Correct rate Case:", float(total_words - error_cases) / total_words)

        error_numbers = 0
        word_index = 0
        for pred_word, real_word in zip(pred_class_number, Ynumbers):
            if pred_word != real_word:
                total_error.add(word_index)
                error_numbers += 1
            word_index += 1

        print("Erros added by number:", len(total_error) - old_errors)
        old_errors = len(total_error)

        print("Total words:", total_words)
        print("Error words:", error_numbers)
        print("Error rate numbers:", float(error_numbers) / total_words)
        print("Correct rate numbers:", float(total_words - error_numbers) / total_words)

        error_genders = 0
        word_index = 0
        for pred_word, real_word in zip(pred_class_gender, Ygenders):
            if pred_word != real_word:
                total_error.add(word_index)
                error_genders += 1
            word_index += 1

        print("Erros added by gender:", len(total_error) - old_errors)
        old_errors = len(total_error)

        print("Total words:", total_words)
        print("Error words:", error_genders)
        print("Error rate genders:", float(error_genders) / total_words)
        print("Correct rate genders:", float(total_words - error_genders) / total_words)

        error_tences = 0
        word_index = 0
        for pred_word, real_word in zip(pred_class_tense, Ytences):
            if pred_word != real_word:
                total_error.add(word_index)
                error_tences += 1
            word_index += 1

        print("Erros added by tense:", len(total_error) - old_errors)
        old_errors = len(total_error)

        print("Total words:", total_words)
        print("Error words:", error_tences)
        print("Error rate tences:", float(error_tences) / total_words)
        print("Correct rate tences:", float(total_words - error_tences) / total_words)

        print("Total error words:", len(total_error))
        print("Total correctness:", float(total_words - len(total_error)) / total_words)

        reverse_mapping = {v: k for k, v in PARTS_MAPPING.items()}

        def classify_morphem_handmande():
            morphem_predictions = predictions[5:]
            print("Morphem predictions shape", morphem_predictions[0].shape)
            print("Morphem predictions", morphem_predictions[0][0:10])
            morphem_classes = [pred.argmax(axis=-1) for pred in morphem_predictions]
            print("Morphem classes", morphem_classes[0][0:10])
            result = []
            for i, batch in enumerate(words):
                word = batch[0]
                word_text = word.get_word()
                raw_parse = []
                for j, letter in enumerate(word_text):
                    predicted_class = morphem_classes[j][i]
                    raw_parse.append(reverse_mapping[int(predicted_class)])
                parse = _transform_classification(raw_parse)
                result.append(parse)
            print(measure_quality(result, [w[0].get_labels() for w in words], [w[0].get_word() for w in words], True))

        def classify_morphem():
            pred_class = predictions[5].argmax(axis=-1)
            result = []
            for i, batch in enumerate(words):
                word = batch[0]
                word_text = word.get_word()
                cutted_prediction = pred_class[i][:len(word_text)]
                raw_parse = [reverse_mapping[int(num)] for num in cutted_prediction]
                parse = _transform_classification(raw_parse)
                result.append(parse)
            print(measure_quality(result, [w[0].get_labels() for w in words], [w[0].get_word() for w in words], False))

        classify_morphem()

if __name__ == "__main__":
    train_txt = prepare_dataset("./datasets/labeled_gikry_better_train.txt", 1)
    test_txt = prepare_dataset("./datasets/labeled_gikry_better_test.txt", 1)

    model = JoinedModel(1, 55, 0.1)
    model.train(train_txt)
    model.classify(test_txt)
