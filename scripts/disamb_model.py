from tensorflow.keras.models import Sequential, Model
from tensorflow.keras.layers import LSTM, Bidirectional, Conv1D, Flatten
from tensorflow.keras.layers import Dense, Input, Concatenate, Masking
from tensorflow.keras.layers import TimeDistributed, Dropout, BatchNormalization
from tensorflow.keras.utils import to_categorical
import numpy as np

from keras.preprocessing.sequence import pad_sequences
from keras.callbacks import EarlyStopping
import time
import pyxmorphy
from pyxmorphy import UniSPTag, UniMorphTag
import fasttext

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

BATCH_SIZE = 9
EMBED_SIZE = 50

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
embedder = fasttext.load_model("morphorueval_cbow.embedding_{}.bin".format(EMBED_SIZE))
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


def get_subsentences_from_long_sentence(sentence):
    tail = sentence[-BATCH_SIZE:]
    subsentences = []
    while len(sentence) > BATCH_SIZE:
        subsentences.append(sentence[:BATCH_SIZE])
        sentence = sentence[BATCH_SIZE:]
    subsentences.append(tail)
    return subsentences


def prepare_dataset(path, trim):
    result = []
    sentence = []
    with open(path, 'r') as f:
        i = 0
        for line in f:
            i += 1
            line = line.strip()
            if not line:
                if len(sentence) <= BATCH_SIZE:
                    while len(sentence) < BATCH_SIZE:
                        sentence.append(("", "X", "_", "_", "_", "_", "_"))
                    result += sentence
                else:
                    for subsent in get_subsentences_from_long_sentence(sentence):
                        result += subsent
                sentence = []
            else:
                splited = line.split('\t')
                tags = splited[4].split('|')
                case = '_'
                number = '_'
                gender = '_'
                tense = '_'
                #animacy = '_'
                for tag in tags:
                    if tag.startswith('Case='):
                        case = tag
                    elif tag.startswith('Number='):
                        number = tag
                    elif tag.startswith('Gender='):
                        gender = tag
                    elif tag.startswith('Tense='):
                        tense = tag
                    #elif tag.startswith('Animacy='):
                    #    animacy = tag
                sentence.append((splited[1], splited[3], case, number, gender, tense))
            if i % 1000 == 0:
                print("Readed:", i)

    return result[:int(len(result) * trim)]


def vectorize_dataset(dataset):
    train_encoded = []
    target_sp_encoded = []
    target_case_encoded = []
    target_number_encoded = []
    target_gender_encoded = []
    target_tense_encoded = []
    #target_animacy_encoded = []

    i = 0
    for word in dataset:
        i += 1
        analyzer_result = None
        if word[0]:
            analyzer_result = analyzer.analyze(word[0], False)[0]
        word_vector = embedder.get_word_vector(word[0])
        speech_part_vector = build_speech_part_array(analyzer_result)
        case_part_vector = build_case_array(analyzer_result)
        number_vector = build_number_array(analyzer_result)
        gender_vector = build_gender_array(analyzer_result)
        tense_vector = build_tense_array(analyzer_result)
        #animacy_vector = build_animacy_array(analyzer_result)
        if word[1] not in speech_part_mapping:
            continue
        train_encoded.append(list(word_vector) + speech_part_vector + case_part_vector + number_vector + gender_vector + tense_vector)
        target_sp_encoded.append(to_categorical(speech_part_mapping[word[1]], num_classes=len(SPEECH_PARTS)).tolist())
        target_case_encoded.append(to_categorical(case_mapping[word[2]], num_classes=len(case_mapping)).tolist())
        target_number_encoded.append(to_categorical(number_mapping[word[3]], num_classes=len(number_mapping)).tolist())
        target_gender_encoded.append(to_categorical(gender_mapping[word[4]], num_classes=len(gender_mapping)).tolist())
        target_tense_encoded.append(to_categorical(tense_mapping[word[5]], num_classes=len(tense_mapping)).tolist())
        #target_animacy_encoded.append(to_categorical(animacy_mapping[word[6]], num_classes=len(animacy_mapping)).tolist())
        if i % 1000 == 0:
            print("Vectorized:", i)

    return train_encoded, target_sp_encoded, target_case_encoded, target_number_encoded, target_gender_encoded, target_tense_encoded,# target_animacy_encoded


def _chunks(lst, n):
    result = []
    for i in range(0, len(lst), n):
        result.append(lst[i:i + n])
    return result


def batchify_dataset(xs, ys1, ys2, ys3, ys4, ys5, batch_size):
    return (
        pad_sequences(_chunks(xs, batch_size), padding='post', dtype=np.int8, maxlen=batch_size),
        pad_sequences(_chunks(ys1, batch_size), padding='post', dtype=np.int8, maxlen=batch_size),
        pad_sequences(_chunks(ys2, batch_size), padding='post', dtype=np.int8, maxlen=batch_size),
        pad_sequences(_chunks(ys3, batch_size), padding='post', dtype=np.int8, maxlen=batch_size),
        pad_sequences(_chunks(ys4, batch_size), padding='post', dtype=np.int8, maxlen=batch_size),
        pad_sequences(_chunks(ys5, batch_size), padding='post', dtype=np.int8, maxlen=batch_size),
        #pad_sequences(_chunks(ys6, batch_size), padding='post', dtype=np.int8, maxlen=batch_size),
        )



class DisambModel(object):
    def __init__(self, dropout, layers, models_number, epochs, validation_split, window_sizes):
        self.dropout = dropout
        self.layers = layers
        self.models_number = models_number
        self.epochs = epochs
        self.validation_split = validation_split
        self.window_sizes = window_sizes
        self.activation = "softmax"
        self.optimizer = "adam"
        self.models = []

    def _build_model(self):
        inp = Input(shape=(BATCH_SIZE, EMBED_SIZE + len(SPEECH_PARTS) + len(CASE_TAGS) + len(NUMBER_TAGS) + len(GENDER_TAGS) + len(TENSE_TAGS)
                           #+ len(ANIMACY_TAGS),
        ))
        inputs = [inp]
        do = None
        inp = BatchNormalization()(inp)

        conv_outputs = []
        for drop, units, window_size in zip(self.dropout, self.layers, self.window_sizes):
            conv = Conv1D(units, window_size, activation='relu', padding="same")(inp)
            norm = BatchNormalization()(conv)
            do = Dropout(drop)(norm)
            inp = do
            conv_outputs.append(do)

        conv_output = Concatenate(name="conv_output")(conv_outputs)
        sp_output = TimeDistributed(Dense(len(SPEECH_PARTS), activation=self.activation), name="speech_part")(conv_output)
        concat_sp_out = Concatenate(name="concat_sp_out")([conv_output, sp_output])
        case_output = TimeDistributed(Dense(len(CASE_TAGS), activation=self.activation), name="case")(concat_sp_out)
        number_output = TimeDistributed(Dense(len(NUMBER_TAGS), activation=self.activation), name="number")(concat_sp_out)
        gender_output = TimeDistributed(Dense(len(GENDER_TAGS), activation=self.activation), name="gender")(concat_sp_out)
        tense_output = TimeDistributed(Dense(len(TENSE_TAGS), activation=self.activation), name="tense")(concat_sp_out)
        #animacy_output = TimeDistributed(Dense(len(ANIMACY_TAGS), activation=self.activation), name="animacy")(concat_sp_out)

        outputs = [sp_output, case_output, number_output, gender_output, tense_output,
       #            animacy_output
        ]
        self.models.append(Model(inputs, outputs=outputs))
        self.models[-1].compile(loss='categorical_crossentropy',
                                optimizer=self.optimizer, metrics=['acc'])
        print(self.models[-1].summary())

    def train(self, words):
        xs, y_sp, y_case, y_number, y_gender, y_tense = vectorize_dataset(words)
        Xs, Y_sp, Y_case, Y_number, Y_gender, Y_tense = batchify_dataset(xs, y_sp, y_case, y_number, y_gender, y_tense, BATCH_SIZE)
        for i in range(self.models_number):
            self._build_model()
        es1 = EarlyStopping(monitor='val_speech_part_acc', patience=10, verbose=1)
        es2 = EarlyStopping(monitor='val_case_acc', patience=10, verbose=1)
        for i, model in enumerate(self.models):
            model.fit(Xs, [Y_sp, Y_case, Y_number, Y_gender, Y_tense], epochs=self.epochs, verbose=2,
                      callbacks=[es1, es2], validation_split=self.validation_split)
            model.save("keras_model_em_50_{}.h5".format(int(time.time())))

    def classify(self, words):
        print("Total models:", len(self.models))
        Xs_, Y_SP_, Y_CASE_, Y_NUMBER_, Y_GENDER_, Y_TENSE_ = vectorize_dataset(words)
        Xs, Y_SP, Y_CASE, Y_NUMBER, Y_GENDER, Y_TENSE = batchify_dataset(Xs_, Y_SP_, Y_CASE_, Y_NUMBER_, Y_GENDER_, Y_TENSE_, batch_size=BATCH_SIZE)
        pred_sp, pred_case, pred_number, pred_gender, pred_tense = self.models[0].predict(Xs)
        pred_class_sp = pred_sp.argmax(axis=-1)
        pred_class_case = pred_case.argmax(axis=-1)
        pred_class_number = pred_number.argmax(axis=-1)
        pred_class_gender = pred_gender.argmax(axis=-1)
        pred_class_tense = pred_tense.argmax(axis=-1)
        #pred_class_animacy = pred_animacy.argmax(axis=-1)
        Ysps = Y_SP.argmax(axis=-1)
        Ycases = Y_CASE.argmax(axis=-1)
        Ynumbers = Y_NUMBER.argmax(axis=-1)
        Ygenders = Y_GENDER.argmax(axis=-1)
        Ytences = Y_TENSE.argmax(axis=-1)
        #Yanimacy = Y_ANIMACY.argmax(axis=-1)
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
        #print("Predicted Animacy")
        #print(pred_class_animacy[:10])
        #print("Actually Animacy")
        #print(Yanimacy[:10])

        total_error = set([])
        total_words = len(Ysps) * BATCH_SIZE

        error_sps = 0
        word_index = 0
        for pred_sent, real_sent in zip(pred_class_sp, Ysps):
            for pred_word, real_word in zip(pred_sent, real_sent):
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
        for pred_sent, real_sent in zip(pred_class_case, Ycases):
            for pred_word, real_word in zip(pred_sent, real_sent):
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
        for pred_sent, real_sent in zip(pred_class_number, Ynumbers):
            for pred_word, real_word in zip(pred_sent, real_sent):
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
        for pred_sent, real_sent in zip(pred_class_gender, Ygenders):
            for pred_word, real_word in zip(pred_sent, real_sent):
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
        for pred_sent, real_sent in zip(pred_class_tense, Ytences):
            for pred_word, real_word in zip(pred_sent, real_sent):
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

        #error_animacy = 0
        #word_index = 0
        #for pred_sent, real_sent in zip(pred_class_animacy, Yanimacy):
        #    for pred_word, real_word in zip(pred_sent, real_sent):
        #        if pred_word != real_word:
        #            total_error.add(word_index)
        #            error_animacy += 1
        #        word_index += 1

        #print("Erros added by animacy:", len(total_error) - old_errors)
        #old_errors = len(total_error)

        #print("Total words:", total_words)
        #print("Error words:", error_animacy)
        #print("Error rate animacy:", float(error_animacy) / total_words)
        #print("Correct rate animacy:", float(total_words - error_animacy) / total_words)

        print("Total error words:", len(total_error))
        print("Total correctness:", float(total_words - len(total_error)) / total_words)


if __name__ == "__main__":
    train_txt = prepare_dataset("./datasets/gikrya.train1", 1)
    test_txt = prepare_dataset("./datasets/gikrya.test1", 1)

    model = DisambModel([0.4, 0.3, 0.2], [512, 256, 192], 1, 75, 0.1, [3, 3, 3])
    model.train(train_txt)
    model.classify(test_txt)
