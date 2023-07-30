import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import tensorflow as tf
import tensorflow_datasets as tfds
import tensorflow.keras as keras
from tensorflow.keras.layers import Conv2D, MaxPooling2D, Dense
from tensorflow.keras.layers import Activation, BatchNormalization, Flatten
from tensorflow.keras.models import Sequential
from tensorflow.keras.utils import to_categorical
from sklearn.model_selection import train_test_split

df = pd.read_excel("D:\willy\OneDrive - 長庚大學\Git\ARC2023\data\_all0to6km.xlsx")

speed = df.iloc[1:, 0].values
p_sc = df.iloc[1:, 1].values
dist = df.iloc[1:, 2].values
thro = df.iloc[1:, 3].values

X = np.vstack((speed, p_sc, dist))

y = thro
X = np.transpose(X)
y = np.transpose(y)

X_train, X_val, y_train, y_val = train_test_split(X, y, test_size=0.5)

model = Sequential()
model.add(Dense(5 ,input_shape=[3]))
model.add(BatchNormalization())
model.add(Activation("relu"))
model.add(Dense(1))
print(model.summary())
model.compile(optimizer='adam',
              loss=tf.keras.losses.MeanSquaredError(),
              )

hisory = model.fit(X_train, y_train,
          verbose = 1,
          epochs = 300
          )

pred = model.predict(X_val)
model.save('mlp_dense5')