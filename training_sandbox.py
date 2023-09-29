import pandas as pd
import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt
import pickle
import datetime

INPUT_WINDOW_SIZE = 5
BATCH_SIZE = 32
CHECKPOINT_DIR = './training_checkpoints'

def load_data():
    with open("test.pkl", "rb") as f:
        data = pickle.load(f)

    rename_column = lambda df: df[1][["Close"]].rename(
                                columns={"Close":df[0]["종목명"]})

    combined = pd.concat([rename_column(ii) for ii in data], axis=1)
    combined = combined.fillna(0).astype(int)

    np_comb = np.array(combined)
    return np_comb

def create_binary_outputs(batch):
    last_two = batch[:, -2:, :]
    increased = tf.math.greater(last_two[:, 1, :], last_two[:,0,:])
    binary_output = tf.cast(increased, dtype=tf.float32)
    return batch[:, :-1, :], binary_output

def prepare_dataset(dataset_base):
    dataset = tf.keras.utils.timeseries_dataset_from_array(
        data = dataset_base,
        targets = None,
        sequence_length = INPUT_WINDOW_SIZE+1,
        sequence_stride = 1,
        shuffle = True,
        batch_size = BATCH_SIZE,
    )
    dataset = dataset.map(create_binary_outputs)
    return dataset

def create_dataset(np_data_base):
    train_ds_base = np_data_base[:7000]
    test_ds_base = np_data_base[7000:]
    ds_train = prepare_dataset(train_ds_base).take(220)
    ds_test = prepare_dataset(test_ds_base).take(30)
    return ds_train, ds_test

def train_model(model, loss_fn, metrics, callbacks):
    np_data_base = load_data()
    ds_train, ds_test = create_dataset(np_data_base)

    model.compile(
        optimizer = tf.keras.optimizers.Adam(0.001),
        loss = loss_fn,
        metrics = metrics
    )

    model.fit(
        ds_train,
        epochs = 30,
        validation_data = ds_test,
        callbacks=callbacks
    )

model = tf.keras.models.Sequential([
    tf.keras.layers.Flatten(input_shape=(INPUT_WINDOW_SIZE,2754)),
    tf.keras.layers.Dense(2754*INPUT_WINDOW_SIZE, activation = "relu"),
    tf.keras.layers.Dense(10_000, activation = "relu"),
    tf.keras.layers.Dense(8000, activation = "relu"),
    tf.keras.layers.Dense(2754, activation = "sigmoid"),
])

loss_fn = tf.keras.losses.BinaryCrossentropy()
metrics = [tf.keras.metrics.BinaryAccuracy()]

log_dir = "logs/fit/" + datetime.datetime.now().strftime("%Y%m%d-%H%M%S")
callbacks = [tf.keras.callbacks.ModelCheckpoint(
                filepath=CHECKPOINT_DIR + "/ckpt", 
                save_best_only=True, 
                monitor='val_binary_accuracy', 
                verbose=1, 
                mode='auto', 
                save_freq='epoch'
            ),
            tf.keras.callbacks.TensorBoard(log_dir=log_dir, histogram_freq=1)
]