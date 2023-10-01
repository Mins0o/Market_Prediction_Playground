import pandas as pd
import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt
import pickle
import datetime

CHECKPOINT_DIR = './training_checkpoints'

def load_data():
    with open("Change.pkl", "rb") as f:
        data = pickle.load(f)
    data=data.fillna(0)
    return data


def train_model(model, loss_fn, metrics, callbacks):
    np_data_base = np.array(load_data())
    ds_train = np_data_base[:-1].reshape((-1,1,3446))
    ds_test = np_data_base[1:]

    model.compile(
        optimizer = tf.keras.optimizers.Adam(0.01),
        loss = loss_fn,
        metrics = metrics
    )

    model.fit(
        ds_train,
        ds_test,
        epochs = 1000,
        batch_size = 1000,
        validation_split = 0.1,
        callbacks=callbacks
    )

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


model = tf.keras.models.Sequential([
    tf.keras.layers.LSTM(500, return_sequences=True, input_shape=(1, 3446)),
    tf.keras.layers.LSTM(500),
    tf.keras.layers.Dense(3446, activation = create_custom_activation())
])

loss_fn = tf.keras.losses.MeanSquaredError()
metrics = [tf.keras.metrics.MeanAbsoluteError()]

log_dir = "logs/fit/" + datetime.datetime.now().strftime("%Y%m%d-%H%M%S")
callbacks = [tf.keras.callbacks.ModelCheckpoint(
                filepath=CHECKPOINT_DIR + "/ckpt", 
                save_best_only=True, 
                verbose=1, 
                mode='auto', 
                save_freq='epoch'
            ),
            tf.keras.callbacks.TensorBoard(log_dir=log_dir, histogram_freq=1)
]

if __name__ == "__main__":
    train_model(model, loss_fn, metrics, callbacks)