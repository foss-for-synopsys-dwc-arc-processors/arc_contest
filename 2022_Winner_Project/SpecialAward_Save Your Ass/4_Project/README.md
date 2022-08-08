<h1 align="center">
Save your ass
</h1>

## Introduction
This project is to design a smart cushion that can help one to sit straight and prevent the users from being sedentary. To help users know more about there sitting poses, we also develop the app for users to trace their sitting condition.

### Code of EM9D
To run the code on EM9D, users should program the image file into the EM9D board. The folder `EM9D` includes the code for running the program on board. EM9D is used to do classify the sensor data into six different categories of sitting pose and send the prediction back to RPI.
```
Process
---------------------------
| -> 1. Send Connection
        |
        |
        | 2. Wait for response
        |
        v
| -> 3. Get Response
|
| - - -> 4. Start receiving - - - - -
|                                   |
|                                   |
|    Loop (Back to 4)               |
∧                                   |
|                                   v
 - - - 5. Predict and send back <- - 
```

### RPI code
RPI is used to collect the pressure data from the sensor and normalized the data to `-128 ~ 127` before sending to EM9D board with UART.

## Setup

### EM9D
In order to exectute the code, user can directly make the file and create the flash:
```shell
$ make
$ make flash
```
The image file will appear in the `output` folder placed in the SDK root.

### RPI
#### a. Install the requirements
```shell
$ pip install -r RPI/requirements.txt
```

#### b. Add Firebase Authentication
Set up the firebase authentication from [here](http://firebase.google.com). Users should download the authentication file and put it into the `RPI` folder.

There are some problems for running the `firebase-admin` package, to solve the issues run the below commands:
```shell
$ pip install firebase_admin
$ pip install grpcio==1.44.0 grpcio-status==1.44.0
$ cd ~/.local/lib/python3.9/site-packages/grpc/_cython
$ patchelf cygrpc.cpython-39-arm-linux-gnueabihf.so \
  --add-needed /lib/arm-linux-gnueabihf/libatomic.so.1
```

#### c. Execution
```shell
$ python RPI/sensor.py
```

### App
User can directly open the `app` folder and execture the code. To generate the apk manually, user can run:
```shell
# Users can specify `debug` or `release` version
$ flutter build apk --<version>
```

### Model Preparation

#### a. Dataset Collection
We collect the data by our team members with eight pressure sensors, and for each category, we collect 20 data from five different team members. The six categories included:
1. Normal
2. Leaning back
3. Left sitting
4. Right sitting
5. Crossed left leg
6. Crossed right leg

Our data is put in the `data` folder, and the filename represents the data class. The `i`th column name shows the pressure of the `i`th sensor.
```csv
one,two,three,four,five,six,seven,eight
44,8,50,99,97,45,30,38
36,3,49,133,101,44,31,37
...
```

To collect the data, execute `RPI/collect.py`.

#### b. Training
We use jupyter notebook to training our model. Users can access to our training code in `pose.ipynb`. Our mdoel architecture is shown below:

<center>
  <img src="assets/valid.svg" alt="training curve" />
  <img src="assets/model.jpg" alt="model"/>
</center>

#### c. Quantization
After training the model, we quantize the model to `int8` to ensure it can be used in our board. One thing to notice is that during the training process, we approximate our input value to integer to reduce the influence of quantization. Addition to **Post Quantization (PQ)**, we also implement **Quantization Aware Training (QAT)** to improve the accuracy and stabiliy during the inference stage.

| Method | Accuracy | Stability |
|:------:|:--------:|:---------:|
|   PQ   |  99.81%  |    low    |
|  QAT   |   100%   |    high   |

The table above shows little differences between **PQ** and **QAT** because our input data is designed to be near integer (still in `float32 type`). However, the stability of our model increased dramatically after deploying on EM9D.

After generating the `.tflite`, user can generate the `model.h` with:
```shell
$ xxd -i generated/pose.tflite > model.h
```


## User Manual
Using our product is quite simple, users just need to buy our product and plug in the device. Then our product can starting running. If users want to setup our project from scratch, please refer to [Setup](#setup).

## Demonstration
1. [Link 1](https://youtu.be/EHoH52kiYqw)
2. [Link 2](https://drive.google.com/drive/folders/18BanDFGr8V2MhJQJmjYqbN_WAPSvKNdI?usp=sharing)
