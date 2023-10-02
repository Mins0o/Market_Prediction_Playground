import matplotlib.pyplot as plt
import tensorflow as tf
import numpy as np
import pickle
from os.path import join as path_join

CHECKPOINT_DIR = 'training_checkpoints'
CHECKPOINT_PATH = r"ckpt_01150\variables\variables"

def load_data():
    with open(path_join("data_pkl","Change.pkl"), "rb") as f:
        data = pickle.load(f)
    data=data.fillna(0)
    return data

def load_full_data():
    with open(path_join("data_pkl","KRX_data.pkl"),"rb") as f:
        full_data =pickle.load(f)
    return full_data

def create_custom_activation():
    def activation(x):
        eps = 0.1
        slope = 0.05 
        logit = lambda x: tf.math.log(x / (1 - x + eps))
        push_aside = logit(eps/(30+eps))
        positive_new_sigmoid = (30+eps) * tf.sigmoid(x*slope + push_aside) - eps
        negative_new_sigmoid = -(30+eps) * tf.sigmoid(-x*slope + push_aside) + eps
        return tf.where(x > 0, positive_new_sigmoid, negative_new_sigmoid)
    return activation

def create_model():
    model = tf.keras.models.Sequential([
        tf.keras.layers.LSTM(300, return_sequences=True, input_shape=(1, 3446)),
        tf.keras.layers.Dropout(0.2),
        tf.keras.layers.LSTM(200),
        tf.keras.layers.Dropout(0.2),
        tf.keras.layers.Dense(5000, activation = "relu"),
        tf.keras.layers.Dropout(0.2),
        tf.keras.layers.Dense(3446, activation = create_custom_activation())
    ])
    return model

def load_saved_model(checkpoint_path):
    new_model = create_model()
    path = path_join(CHECKPOINT_DIR,checkpoint_path)
    print(path)
    new_model.load_weights(path)
    new_model.summary()
    return new_model

loaded_model = load_saved_model(checkpoint_path = CHECKPOINT_PATH)
change_data = load_data()
np_ch = np.array(change_data).reshape((-1,1,3446))

def test_maxima():
    input_data = np_ch[8000:8100]
    output = loaded_model(input_data)[-1]
    truth = np_ch[8100]

