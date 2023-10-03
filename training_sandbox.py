import pandas as pd
import numpy as np
import tensorflow as tf
import pickle
import datetime
from os.path import join as path_join

CHECKPOINT_DIR = './training_checkpoints'

def load_data():
    with open(path_join("data_pkl","Change.pkl"), "rb") as f:
        data = pickle.load(f)
    data=data.fillna(0)
    return data

def train_model(model, loss_fn, metrics, callbacks):
    np_data_base = np.array(load_data())
    ds_train = np_data_base[:-1].reshape((-1,1,3446))
    ds_test = np_data_base[1:]

    model.compile(
        optimizer = tf.keras.optimizers.Adam(0.1),
        loss = loss_fn,
        metrics = metrics
    )

    model.fit(
        ds_train,
        ds_test,
        epochs = 5000,
        batch_size = 150,
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

def create_model():
    model = tf.keras.models.Sequential([
        tf.keras.layers.LSTM(3000, return_sequences=True, input_shape=(1, 3446)),
        tf.keras.layers.Dropout(0.2),
        tf.keras.layers.LSTM(1000),
        tf.keras.layers.Dropout(0.2),
        tf.keras.layers.Dense(3000, activation = "relu"),
        tf.keras.layers.Dropout(0.2),
        tf.keras.layers.Dense(3446, activation = create_custom_activation())
    ])
    model.summary()
    return model

def load_saved_model(checkpoint_path):
    new_model = create_model()
    path = path_join(CHECKPOINT_DIR,checkpoint_path)
    new_model.load_weights(path)
    return new_model

model = create_model()
loss_fn = tf.keras.losses.MeanSquaredError()
metrics = [tf.keras.metrics.MeanAbsoluteError()]

log_dir = path_join("logs","fit", datetime.datetime.now().strftime("%Y%m%d-%H%M%S"))
callbacks = [tf.keras.callbacks.ModelCheckpoint(
                filepath=path_join(CHECKPOINT_DIR, "ckpt_best"), 
                save_best_only=True, 
                verbose=1, 
                mode='auto', 
                save_freq='epoch'
            ),
            tf.keras.callbacks.ModelCheckpoint(
                filepath=path_join(CHECKPOINT_DIR,
                                   "ckpt_{epoch:05d}"), 
                verbose=0, 
                mode='auto', 
                save_freq=53*50
            ),
            tf.keras.callbacks.TensorBoard(log_dir=log_dir, histogram_freq=1)
]

if __name__ == "__main__":
    # train_model(model, loss_fn, metrics, callbacks)
    # model = load_saved_model(path_join("ckpt_02450","variables","variables"))
    train_model(model, loss_fn, metrics, callbacks)