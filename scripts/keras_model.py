from tensorflow.keras.models import Sequential, Model
from tensorflow.keras.layers import LSTM, Bidirectional, Conv1D, Flatten
from tensorflow.keras.layers import Dense, Input, Concatenate, Masking
from tensorflow.keras.layers import TimeDistributed, Dropout
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

BATCH_SIZE = 7
EMBED_SIZE = 50

CASE_TAGS = [
    
]


analyzer = pyxmorphy.MorphAnalyzer()
embedder = fasttext.load_model("morphorueval_cbow.embedding_{}.bin".format(EMBED_SIZE))
speech_part_len = len(SPEECH_PARTS)
speech_part_mapping = {str(s): num for num, s in enumerate(SPEECH_PARTS)}


def build_speech_part_array(analyzer_results):
    output = [0 for _ in range(speech_part_len)]
    for result in analyzer_results.infos:
        output[speech_part_mapping[str(result.sp)]] = 1
    return output


def build_speech_part_answer(sp):
    output = [0 for _ in range(speech_part_len)]
    output[speech_part_mapping[str(sp)]] = 1
    return output


def prepare_dataset(path, trim):
    result = []
    with open(path, 'r') as f:
        i = 0
        for line in f:
            i += 1
            line = line.strip()
            if not line:
                continue
            else:
                splited = line.split('\t')
                result.append((splited[1], splited[3]))
            if i % 1000 == 0:
                print("Readed:", i)

    return result[:int(len(result) * trim)]


def vectorize_dataset(dataset):
    train_encoded = []
    target_encoded = []

    i = 0
    for word in dataset:
        i += 1
        analyzer_result = analyzer.analyze(word[0], False)[0]
        word_vector = embedder.get_word_vector(word[0])
        speech_part_vector = build_speech_part_array(analyzer_result)
        if word[1] not in speech_part_mapping:
            continue
        train_encoded.append(list(word_vector) + speech_part_vector)
        target_encoded.append(to_categorical(speech_part_mapping[word[1]], num_classes=len(SPEECH_PARTS)).tolist())
        if i % 1000 == 0:
            print("Vectorized:", i)

    return train_encoded, target_encoded


def _chunks(lst, n):
    result = []
    for i in range(0, len(lst), n):
        result.append(lst[i:i + n])
    return result


def batchify_dataset(xs, ys, batch_size):
    return pad_sequences(_chunks(xs, batch_size), padding='post', dtype=np.int8, maxlen=batch_size), pad_sequences(_chunks(ys, batch_size), padding='post', dtype=np.int8, maxlen=batch_size)



class LSTMModel(object):
    def __init__(self, dropout, layers, models_number, epochs, validation_split, batch_size):
        self.dropout = dropout
        self.layers = layers
        self.models_number = models_number
        self.epochs = epochs
        self.validation_split = validation_split
        self.batch_size = batch_size
        self.activation = "softmax"
        self.optimizer = "adam"
        self.models = []

    def _build_model(self):
        inp = Input(shape=(self.batch_size, EMBED_SIZE + 20))
        inputs = [inp]
        prev_do = None
        for drop, units in zip(self.dropout, self.layers):
            bidi = Bidirectional(LSTM(units, return_sequences=True))(inp)
            do = Dropout(drop)(bidi)
            if prev_do is None:
                inp = Concatenate(axis=-1)([do, inp])
            else:
                inp = Concatenate(axis=-1)([do, prev_do])
            prev_do = do


        outputs = [TimeDistributed(
            Dense(len(SPEECH_PARTS) + 1, activation=self.activation))(inp)]
        self.models.append(Model(inputs, outputs=outputs))
        self.models[-1].compile(loss='categorical_crossentropy',
                                optimizer=self.optimizer, metrics=['val_acc'])

    def train(self, words):
        x_, y_ = vectorize_dataset(words)
        Xs, Ys = batchify_dataset(x_, y_, self.batch_size)

        for i in range(self.models_number):
            self._build_model()
        es = EarlyStopping(monitor='val_acc', patience=10, verbose=1)
        for i, model in enumerate(self.models):
            model.fit(Xs, Ys, epochs=self.epochs, verbose=2,
                      callbacks=[es], validation_split=self.validation_split)
            model.save("keras_model_50em_sp_{}.h5".format(int(time.time())), include_optimizer=False)

    def classify(self, words):
        print("Total models:", len(self.models))
        x_, y_ = vectorize_dataset(words)
        Xs, Ys = batchify_dataset(x_, y_, self.batch_size)
        pred = np.mean([model.predict(Xs) for model in self.models], axis=0)
        pred_class = pred.argmax(axis=-1)
        Ys = Ys.argmax(axis=-1)
        print("Predicted")
        print(pred_class[:10])
        print("Actually")
        print(Ys[:10])
        error_words = 0
        for pred_sent, real_sent in zip(pred_class, Ys):
            for pred_word, real_word in zip(pred_sent, real_sent):
                if pred_word != real_word:
                    error_words += 1
        total_words = len(Ys) * self.batch_size
        print("Total words:", total_words)
        print("Error words:", error_words)
        print("Error rate:", float(error_words) / total_words)
        print("Correct rate:", float(total_words - error_words) / total_words)


class CNNModel(object):
    def __init__(self, dropout, layers, models_number, epochs, validation_split, window_size):
        self.dropout = dropout
        self.layers = layers
        self.models_number = models_number
        self.epochs = epochs
        self.validation_split = validation_split
        self.window_size = window_size
        self.activation = "softmax"
        self.optimizer = "adam"
        self.models = []

    def _build_model(self):
        inp = Input(shape=(BATCH_SIZE, EMBED_SIZE + 20,))
        inputs = [inp]
        do = None

        conv_outputs = []
        for drop, units in zip(self.dropout, self.layers):
            conv = Conv1D(units, self.window_size, activation='relu', padding="same")(inp)
            do = Dropout(drop)(conv)
            inp = do
            conv_outputs.append(do)

        conv_output = Concatenate(name="conv_output")(conv_outputs)
        outputs = [TimeDistributed(Dense(len(SPEECH_PARTS), activation=self.activation))(conv_output)]
        self.models.append(Model(inputs, outputs=outputs))
        self.models[-1].compile(loss='categorical_crossentropy',
                                optimizer=self.optimizer, metrics=['acc'])
        print(self.models[-1].summary())

    def train(self, words):
        xs, ys = vectorize_dataset(words)
        Xs, Ys = batchify_dataset(xs, ys, BATCH_SIZE)
        for i in range(self.models_number):
            self._build_model()
        es = EarlyStopping(monitor='val_acc', patience=10, verbose=1)
        for i, model in enumerate(self.models):
            model.fit(Xs, Ys, epochs=self.epochs, verbose=2,
                      callbacks=[es], validation_split=self.validation_split)
            model.save("keras_model_em_50_{}.h5".format(int(time.time())))

    def classify(self, words):
        print("Total models:", len(self.models))
        Xs_, Ys_ = vectorize_dataset(words)
        Xs, Ys = batchify_dataset(Xs_, Ys_, BATCH_SIZE)
        pred = np.mean([model.predict(Xs) for model in self.models], axis=0)
        pred_class = pred.argmax(axis=-1)
        Ys = Ys.argmax(axis=-1)
        print("Predicted")
        print(pred_class[:10])
        print("Actually")
        print(Ys[:10])
        error_words = 0
        for pred_sent, real_sent in zip(pred_class, Ys):
            for pred_word, real_word in zip(pred_sent, real_sent):
                if pred_word != real_word:
                    error_words += 1
        total_words = len(Ys) * BATCH_SIZE
        print("Total words:", total_words)
        print("Error words:", error_words)
        print("Error rate:", float(error_words) / total_words)
        print("Correct rate:", float(total_words - error_words) / total_words)





if __name__ == "__main__":
    train_txt = prepare_dataset("./datasets/gikrya.train", 1)
    test_txt = prepare_dataset("./datasets/gikrya.test", 1)

    #model = LSTMModel([0.1, 0.1], [512, 512], 1, 20, 0.1, 10)
    model = CNNModel([0.4, 0.2], [512, 256], 1, 75, 0.1, 3)
    model.train(train_txt)
    model.classify(test_txt)
