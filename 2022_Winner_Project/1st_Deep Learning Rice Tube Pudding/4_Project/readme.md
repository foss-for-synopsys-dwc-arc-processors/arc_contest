# ECG_detect_OSA

------------------------------------------
This program is designed to recognize **Obstructive Sleep Apnea**. Using I²C connect MAX86150 with ARC EM9D board, GMA303KU on board to detect user whether fell asleep, Convolutional Neural Networks to recognize these diease and OLED1306 to show result we get from model.

<img width="450" alt="PC2" src="https://user-images.githubusercontent.com/87894572/176604629-1e02e3cf-c78f-47bf-a87c-b511c30e8c5f.png">


* [Introduction](#introduction)
* [Hardware and Software Setup](#hardware-and-software-setup)
	* [Required Hardware](#required-hardware)
		* [Hardware Connection](#hardware-connection)
	* [Required Software](#required-software)
* [User Manual](#user-manual)
	* [Compatible Model](#compatible-model)
	* [Python work for deep learning](#python-work-for-deep-learning)
		* [Loading dataset and pre-processing](#loading-dataset-and-pre-processing)
		* [Setup model](#setup-model)
	* [C work on board](#c-work-on-board)
		* [Load model](#load-model)
		* [OLED1306 part](#oled1306-part)
		* [MAX86150 part](#max86150-part)
		* [Tflitemicro algo](#tflitemicro-algo)
		* [Model setting](#model-setting)
		* [GMA303KU part](#gma303ku-part)
		* [Main fuction](#main-fuction)
	* [Final Product Introduction](#final-product-introduction)


## Introduction
- 1 Using 3-axis accelerometer to determine user whether is go to sleep.
- 2 Using I²C to trasport ECG data to ARC EM9D board.
- 3 Using Convolutional Neural Networks model to recognize real time ECG data.
- 4 Show the result to user.

All hardware are in the picture following:

<img width="450" alt="PC2" src="https://user-images.githubusercontent.com/87894572/176442424-26c242db-f6ff-4690-a84b-176652868726.png">

## Hardware and Software Setup
### Required Hardware
-  ARC EM9D board
-  MAX86150
-  OLED1306

All hardware are in the picture following:

<img width="450" alt="PC2" src="https://user-images.githubusercontent.com/87894572/177003623-f2245325-a6d2-4a58-b78f-5beda8a00746.png">

### Hardware Connection
- ARC EM9D, MAX861150 and OLED1306 connected by wire.
123
### Required Software
- Metaware or ARC GNU Toolset
- Serial port terminal, such as putty, tera-term or minicom
- VirtualBox(Ubuntu 20.04)
- Cygwin64 Terminal

## User Manual

### Compatible Model

1. Download [Apnea-ECG Database](https://physionet.org/content/apnea-ecg/1.0.0/).

1. Use **Tensorflow 2.x** and **Python 3.7 up** to training model.

1. Import module to read **Apnea-ECG Database**.

```python
import wfdb
```
### Python work for deep learning
#### Loading dataset and pre-processing
- Includes module
```python
import wfdb
import numpy as np
import tensorflow
from tensorflow.keras.callbacks import ModelCheckpoint
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Flatten, BatchNormalization,Dropout
from tensorflow.keras.layers import Conv2D, MaxPooling2D
from tensorflow.keras.regularizers import l2
import sklearn
from sklearn.model_selection import train_test_split
from sklearn.metrics import confusion_matrix
import matplotlib.pyplot as plt
```

- Load testing dataset

```python
#Add "apena-database-1.0.0"location in "change_link"

change_link=''

x_test=[]
y_test=[]

test_string=['x01','x02','x03','x04','x05','x06','x07','x08','x09','x10','x11','x12',
             'x13','x14','x15','x16','x17','x18','x19','x20','x21','x22','x23','x24',
             'x25','x26','x27','x28','x29','x30','x31','x32','x33','x34','x35']
             
test_label_count=[522,468,464,481,504,449,508,516,507,509,456,526,505,489,497,514,399,458,486,
                  512,509,481,526,428,509,519,497,494,469,510,556,537,472,474,482]

test_label=np.loadtxt(change_link+'test-dataset-annos.txt',delimiter='\t',dtype=np.str)

tmp=[]

for i in test_label:
    tmp+=list(i)

for i in range(len(tmp)):
    if tmp[i]=='N':
        tmp[i]=0
    else:
        tmp[i]=1
y_test=np.array(tmp,dtype=np.int8)

count=0
for k in test_string:
    record = wfdb.rdrecord(record_name=change_link+k,return_res=16,physical=True)
    ecg_signal = record.p_signal
    ecg_signal=np.delete(ecg_signal,np.s_[((test_label_count[count])*6000):])
    x_test=np.append(x_test,ecg_signal)
    x_test = x_test.astype(np.float32)
    count+=1
```
- Load training dataset
```python
x_train=[]
y_train=[]
list_string=['a01','a02','a03','a04','a05','a06','a07','a08','a09','a10','a11','a12',
             'a13','a14','a15','a16','a17','a18','a19','a20','b01','b02','b03','b04',
             'b05','c01','c02','c03','c04','c05','c06','c07','c08','c09','c10']

for k in list_string:

    record = wfdb.rdrecord(record_name=change_link+k,return_res=16,physical=True)
    ann = wfdb.rdann(change_link+k,'apn')

    ecg_signal_label=ann.symbol
    ecg_signal_label=np.array(ecg_signal_label)

    ecg_signal = record.p_signal


    ecg_signal=np.delete(ecg_signal,np.s_[((len(ecg_signal_label)-1)*6000):])
    x_train=np.append(x_train,ecg_signal)
    x_train = x_train.astype(np.float32)

    for i in range(len(ecg_signal_label)):
        if ecg_signal_label[i]=='N':
            ecg_signal_label[i]=0
        else:
            ecg_signal_label[i]=1
    ecg_signal_label=np.array(ecg_signal_label,dtype=np.int8)
    ecg_signal_label=np.delete(ecg_signal_label,np.s_[(len(ecg_signal_label)-1):])
    y_train=np.append(y_train,ecg_signal_label)
```
- Merge two dataset in one
```python
tmp_data=np.append(x_train,x_test,)
tmp_label=np.append(y_train,y_test)
```
- Dataset normalization
```python
tmp_data_max_abs=np.max(np.abs(tmp_data))
print(tmp_data)
tmp_data=tmp_data/tmp_data_max_abs
print(tmp_data)
```
- Split dataset
```python
tmp_data=np.reshape(tmp_data,(int(tmp_data.size/6000),6000,1,1))

x_train, x_test, y_train, y_test = train_test_split(tmp_data, tmp_label, test_size=0.1)
x_train.shape,y_train.shape ,x_test.shape , y_test.shape
```
- Flatten Label
```python
y_train = y_train.flatten()
y_test = y_test.flatten()
```
- Encoder Label
```python
num_classes=2
y_train_encoder = sklearn.preprocessing.LabelEncoder()
y_train_num = y_train_encoder.fit_transform(y_train)
y_train_wide = tensorflow.keras.utils.to_categorical(y_train_num, num_classes)
y_test_num = y_train_encoder.fit_transform(y_test)
y_test_wide = tensorflow.keras.utils.to_categorical(y_test_num, num_classes)
```
#### Setup model
- Use 2D CNN to deal with this problem
```python
model_ecg = Sequential()

model_ecg.add(BatchNormalization(input_shape=(6000,1,1)))

model_ecg.add(Conv2D(8,kernel_size=(2,1),padding="same", activation='relu',strides=2,kernel_regularizer=l2(0.001), bias_regularizer=l2(0.001)))
model_ecg.add(Conv2D(8,kernel_size=(2,1),padding="same", activation='relu',strides=2,kernel_regularizer=l2(0.001), bias_regularizer=l2(0.001)))
model_ecg.add(MaxPooling2D(pool_size=(2,1),padding="same"))

model_ecg.add(Conv2D(16,kernel_size=(2,1),padding="same", activation='relu',strides=2,kernel_regularizer=l2(0.001), bias_regularizer=l2(0.001)))
model_ecg.add(Conv2D(16,kernel_size=(2,1),padding="same", activation='relu',strides=2,kernel_regularizer=l2(0.001), bias_regularizer=l2(0.001)))
model_ecg.add(MaxPooling2D(pool_size=(2,1),padding="same"))

model_ecg.add(Conv2D(32,kernel_size=(2,1),padding="same", activation='relu',strides=2,kernel_regularizer=l2(0.001), bias_regularizer=l2(0.001)))
model_ecg.add(Conv2D(32,kernel_size=(2,1),padding="same", activation='relu',strides=2,kernel_regularizer=l2(0.001), bias_regularizer=l2(0.001)))
model_ecg.add(MaxPooling2D(pool_size=(2,1),padding="same"))

model_ecg.add(Dropout(0.5))

model_ecg.add(BatchNormalization())
model_ecg.add(Flatten())
model_ecg.add(Dropout(0.5))

model_ecg.add(Dense(64,activation='relu'))
model_ecg.add(Dense(2,kernel_regularizer=l2(0.01), bias_regularizer=l2(0.01),activation='softmax'))

model_ecg.summary()
```
- Choose optimizer and loss function
```python
opt = tensorflow.keras.optimizers.Adam(lr=0.001)
model_ecg.compile(loss='binary_crossentropy',
              optimizer=opt,
              metrics=['accuracy'])
```
- Select model's hyper parameter
```python
batch_size = 16
epochs = 300
```

- Get ready to train model
```python
best_weights_filepath_ecg = './best_weights_ecg_32layer.hdf5'
mcp_ecg = ModelCheckpoint(best_weights_filepath_ecg, monitor="val_accuracy",
                      save_best_only=True, save_weights_only=False)
history = model_ecg.fit(x_train, 
                        y_train_wide,
                        batch_size=batch_size,
                        epochs=epochs,
                        verbose=1,
                        validation_split=0.1,
                        callbacks=[mcp_ecg])
```
- Plot accuracy and loss
```python
loss = history.history['loss']
val_loss = history.history['val_loss']

acc=history.history['accuracy']
val_acc=history.history['val_accuracy']

plt.figure(figsize=(6,12))

plt.subplot(2,1,1)

plt.xlabel('Epochs')
plt.ylabel('Loss')
plt.plot(loss, 'blue', label='Training Loss')
plt.plot(val_loss, 'green', label='Validation Loss')
plt.xticks(range(0,epochs)[0::100])
plt.title('Training and Validation Loss vs Epochs')
plt.legend()

plt.subplot(2,1,2)

plt.xlabel('Epochs')
plt.ylabel('Accuracy')
plt.plot(acc, 'blue', label='Training Accuracy')
plt.plot(val_acc, 'green', label='Validation Accuracy')
plt.xticks(range(0,epochs)[0::100])
plt.title('Training and Validation Accuracy vs Epochs')
plt.legend()
plt.savefig("plots_perf.svg")
plt.show()
```
- Check our confusion matrix 
```python
model_ecg.load_weights('best_weights_ecg_32layer.hdf5')
y_pred = model_ecg.predict(x_test)
predict_test=np.argmax(y_pred, axis=1)
predict_test=predict_test.reshape(predict_test.shape[0],1)
cm=confusion_matrix(y_test_num, predict_test)
cm
```
- Check test data accuracy
```python
(cm[1,1]+cm[0,0])/(cm[1,1]+cm[1,0]+cm[0,0]+cm[0,1])
```
- Save module and weight
```python
model_ecg.save_weights('model_weights.h5')
model_ecg.save('model_weights.h5')
```
- Convert 'h5' file to 'tflite' file
```python
import tensorflow as tf
model = tf.keras.models.load_model('model_weights.h5')
converter = tf.lite.TFLiteConverter.from_keras_model(model_ecg)
tflite_model = converter.convert()
converter.inference_input_type, converter.inference_output_type
import pathlib
generated_dir = pathlib.Path("generated/")
generated_dir.mkdir(exist_ok=True, parents=True)
converted_model_file = generated_dir/"ecg_model.tflite"
converted_model_file.write_bytes(tflite_model)  
```
- Check 'tflite' file's accuracy
```python
import tensorflow as tf
max_samples = 17233
converted_model_file="generated/ecg_model.tflite"
interpreter = tf.lite.Interpreter(model_path=str(converted_model_file))
interpreter.allocate_tensors()

# A helper function to evaluate the TF Lite model using "test" dataset.
def evaluate_model(interpreter):
    input_index = interpreter.get_input_details()[0]["index"]
    output_index = interpreter.get_output_details()[0]["index"]
    scale, zero_point = interpreter.get_output_details()[0]['quantization']

    prediction_values = []
    
    for test_image in x_test[:max_samples]:
        # Pre-processing: add batch dimension, quantize and convert inputs to int8 to match with
        # the model's input data format.
        test_image = np.expand_dims(test_image, axis=0) #.astype(np.float32)
        test_image = np.float32(test_image)
        interpreter.set_tensor(input_index, test_image)

        interpreter.invoke()

        # Find the letter with highest probability
        output = interpreter.tensor(output_index)
        result = np.argmax(output()[0])
        prediction_values.append(result)
    
    accurate_count = 0
    for index in range(len(prediction_values)):
        if prediction_values[index] == y_test[index]:
            accurate_count += 1
    accuracy = accurate_count * 1.0 / len(prediction_values)
    return accuracy * 100

print(str(evaluate_model(interpreter)) + "%")
```
- Open **Cygwin64 Terminal** press command
```c
xxd -i model.tflite > model.h
```
And you get .h file to put in ARC EM9D
### C work on board
#### Load model
- Open folder "workshop/Synopsys_SDK_V22/Example_Project/model/inc" and copy "model.h" in it.
#### OLED1306 part

- Open "synopsys_i2c_oled1306.c" and copy this code.

```c
/********************************************************************************
                homemade function
 ********************************************************************************/
#define HeartNUM 22
uint8_t HeartLocation[22][2] = {
    {0, 47}, {0, 52}, {0, 72}, {0, 77}, {1, 42}, 
    {1, 57}, {1, 67}, {1, 82}, {2, 37}, {2, 62}, 
    {2, 87}, {3, 37}, {3, 87}, {4, 42}, {4, 82},
    {5, 47}, {5, 77}, {6, 52}, {6, 72}, {7, 57},
    {7, 62}, {7, 67}};

uint8_t HeartData = 0xff;

void DisplayHeart(void){
    for(int j = 0; j < HeartNUM; j++){
        OLED_SetCursor(HeartLocation[j][0], HeartLocation[j][1]);
        for(int i = 0; i < FONT_SIZE; i++)
            oledSendData(HeartData);
        oledSendData(0x00);
    }
}
#define TickNUM 5

uint8_t TickLocation[TickNUM][2] = {
    {4, 2}, {5, 7}, {4, 12}, {3, 17}, {2, 22}
};

uint8_t TickData = 0xff;


void DisplayTick(void){
    for(int j = 0; j < TickNUM; j++){
        OLED_SetCursor(TickLocation[j][0], TickLocation[j][1]);
        for(int i = 0; i < FONT_SIZE; i++)
            oledSendData(TickData);
        oledSendData(0x00);
    }
}

#define MarNUM 9

uint8_t MarkLocation[MarNUM][2] ={
    {2, 102}, {2, 122}, {3, 107}, {3, 117}, {4, 112},
    {5, 107}, {5, 117}, {6, 102}, {6, 122}
    };

uint8_t MarkData = 0xff;

void DisplayResult(char result){
    OLED_Clear();
    switch (result)
    {
    case 'N':{
        DisplayHeart();
        DisplayTick();
        break;
    }

    case 'Y':{
        DisplayHeart();
        DisplayExclamationMark();
        break;
    }

    case 'R':{
        DisplayReady();
        break;
    }

    default:{
        DisplayHeart();
        break;
    }
    }
}
#define ReadyNum 63
uint8_t ReadyLocation[ReadyNum][2] ={
    {0, 12}, {0, 17}, {0, 22}, {3, 12}, {3, 17}, {3, 22}, {6, 12}, {6, 17}, {6, 22},
    {1, 12}, {2, 12}, {4, 22}, {5, 22},  //13
    {0, 37}, {1, 37}, {2, 37}, {3, 37}, {4, 37}, {5, 37}, {6, 37},
    {6, 42}, {6, 47}, // 9
    {0, 62}, {1, 62}, {2, 62}, {3, 62}, {4, 62}, {5, 62}, {6, 62},
    {0, 87}, {1, 87}, {2, 87}, {3, 87}, {4, 87}, {5, 87}, {6, 87},
    {0, 67}, {0, 72}, {3, 67}, {3, 72}, {6, 67}, {6, 72}, 
    {0, 92}, {0, 97}, {3, 92}, {3, 97}, {6, 92}, {6, 97}, //26
    {0, 112}, {1, 112}, {2, 112}, {3, 112}, {4, 112}, {5, 112}, {6, 112}, 
    {0, 117}, {0, 122}, {0, 127}, {1, 127}, {2, 127}, {3, 117}, {3, 122}, {3, 127}, //15
    };

uint8_t ReadyData = 0xff;

void DisplayReady(void){
    for(int j = 0; j < ReadyNum; j++){
        OLED_SetCursor(ReadyLocation[j][0], ReadyLocation[j][1] - 10);
        for(int i = 0; i < FONT_SIZE; i++)
            oledSendData(ReadyData);
        oledSendData(0x00);
    }
}

#define AHINum 44
uint8_t AHILocation[AHINum][2] = {
    {0, 17}, {0, 22},
    {1, 12}, {2, 12}, {3, 12}, {5, 12}, {6, 12},
    {1, 27}, {2, 27}, {3, 27}, {5, 27}, {6, 27},
    {4, 12}, {3, 17}, {3, 22}, {4, 27}, //16
    {0, 37}, {1, 37}, {2, 37}, {3, 37}, {4, 37}, {5, 37}, {6, 37},
    {0, 47}, {1, 47}, {2, 47}, {3, 47}, {4, 47}, {5, 47}, {6, 47},  
    {3, 42},///15
    {0, 62}, {1, 62}, {2, 62}, {3, 62}, {4, 62}, {5, 62}, {6, 62},
    {6, 57}, {6, 67},
    {0, 57}, {0, 67},///11
    {2, 77}, {4, 77}//2
};
uint8_t AHIData = 0xff;

uint8_t NumLocation[15][2] = {
   {0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5},
   {0, 10}, {1, 10}, {2, 10}, {3, 10}, {4, 10},
   {0, 15}, {1, 15}, {2, 15}, {3, 15}, {4, 15},
};

uint8_t NumData[10][15] ={
    {255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 255, 255, 255, 255, 255,},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255,},
    {255, 0, 255, 255, 255, 255, 0, 255, 0, 255, 255, 255, 255, 0, 255,},
    {255, 0, 255, 0, 255, 255, 0, 255, 0, 255, 255, 255, 255, 255, 255,},
    {255, 255, 255, 0, 0, 0, 0, 255, 0, 0, 255, 255, 255, 255, 255,},
    {255, 255, 255, 0, 255, 255, 0, 255, 0, 255, 255, 0, 255, 255, 255,},
    {255, 255, 255, 255, 255, 255, 0, 255, 0, 255, 255, 0, 255, 255, 255,},
    {255, 0, 0, 0, 0, 255, 0, 0, 0, 0, 255, 255, 255, 255, 255,},
    {255, 255, 255, 255, 255, 255, 0, 255, 0, 255, 255, 255, 255, 255, 255,},
    {255, 255, 255, 0, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 255,},
};

void DisplayAHI(int num){
    OLED_Clear();
    for(int j = 0; j < AHINum; j++){
        OLED_SetCursor(AHILocation[j][0], AHILocation[j][1] - 10);
        for(int i = 0; i < FONT_SIZE; i++)
            oledSendData(AHIData);
        oledSendData(0x00);
    }
    for(int k = 0; k < 2 ; k++){
        for(int j = 0; j < 15; j++){
        OLED_SetCursor(NumLocation[j][0] + 1, 77 + NumLocation[j][1] + k * 20);
        for(int i = 0; i < FONT_SIZE; i++)
            if(k == 0)
                oledSendData(NumData[(num / 10)][j]);
            else
                oledSendData(NumData[(num % 10)][j]);
        oledSendData(0x00);
    }
    }
   
    if(num >= 5 && num < 15){
        OLED_SetCursor(7, 46);
        OLED_DisplayString("Mild OSA");
    }
    else if(num >= 15 && num < 30){
        OLED_SetCursor(7, 20);
        OLED_DisplayString("Moderate OSA");
    }
    else if(num >= 30){
        OLED_SetCursor(7, 40);
        OLED_DisplayString("Severe OSA");
    }
    else{
        OLED_SetCursor(7, 54);
        OLED_DisplayString("NO OSA");
    }
}
```
#### MAX86150 part
- Create "max86150.c" file, in order to send ECG data.
```c
#include "max86150.h"

#define uart_buf_size 100

char uart_buf[uart_buf_size] = {0};
uint8_t buffer[MAX][bytes] = {0};
uint8_t read_buf[20] = {0};
DEV_IIC * iic1_ptr;
DEV_UART * uart0_ptr;
uint32_t count, read = 0;

void InitUART(void){
    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);
    sprintf(uart_buf, "I2C teraterm.log\r\n\n");    
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
}

void InitI2C(void){
    iic1_ptr = hx_drv_i2cm_get_dev(USE_SS_IIC_X);
    iic1_ptr->iic_open(DEV_MASTER_MODE, IIC_SPEED_FAST); 

}

void InitMax86150(void){
    InitI2C();
    uint8_t initial_data_write[8][2] = { 
                                         {0x02, 0x80}, //Interrupt Enable 1
                                         {0x03, 0x04}, //Interrupt Enable 2
                                         {0x08, 0x10}, //FIFO Configuration 
					 {0x09, 0x09}, //FIFO Data Control Register 1
					 {0x0A, 0x00}, //FIFO Data Control Register 2
					 {0x3C, 0x03}, //ECG Configuration 1   //0x02   0x03
					 {0x3E, 0x00}, //ECG Configuration 3   //0x0D  0x00
                                         {0x0D, 0x04}, //System Control;
                                        }; 
    for(int i = 0; i < 8; i++){
        Max86150_WriteData(initial_data_write[i][0], initial_data_write[i][1]);
    }
    board_delay_ms(500);
    RestRead();
}


void Max86150_ReadData(uint8_t addr, uint8_t len, uint8_t * read_buf){
    uint8_t WriteData[2];
    WriteData[0] = addr;

    hx_drv_i2cm_write_data(USE_SS_IIC_X, MAX86150_Address, &WriteData[0], 0, &WriteData[0], 1); 
    hx_drv_i2cm_read_data(USE_SS_IIC_X, MAX86150_Address, &read_buf[0], len);
}

void Max86150_WriteData(uint8_t addr, uint8_t data){
    uint8_t WriteData[2];

    WriteData[0] = addr;
    WriteData[1] = data;
    hx_drv_i2cm_write_data(USE_SS_IIC_X, MAX86150_Address, &WriteData[0], 0, &WriteData[0], 2); 
}

//Read ECG from Max86150
void GetECG(float * data){
    uint8_t write_point;

    Max86150_ReadData(0x04, 1, &read_buf[0]);
    write_point = read_buf[0];

    int NumAvailableSamples = write_point - read;
   
    if(NumAvailableSamples < 0){
        NumAvailableSamples += 32;
    }

    if(NumAvailableSamples == 0){
        char test[10] = "NO NO NO"; 
        uart0_ptr->uart_write(test, strlen(test));
    }

    for(int i = 0; i < NumAvailableSamples; i++){
       Max86150_ReadData(0x07, 3, &read_buf[0]);
 
       int buf = ((read_buf[0] << 16) | (read_buf[1] << 8) | read_buf[2]) & 0x3ffff;
          
       if(buf > 0x20000){
          buf -= 0x20000;
          buf = -1 * (0x1ffff - buf + 1);
       }

       data[count] = buf;

       count += 1;
       if(count == MAX)
       {
            break;
       }
    }
    read = write_point;
}


void GetECGloop(float * data){
    
    count = 0;
    while(1){
        board_delay_ms(10);
        GetECG(data);
        if(count == MAX)
        {
            break;
        }
    }
}

//Read ecg FIFO register
void ReadRegister(void){
    uint8_t Interrupt1, Interrupt2, write_point, Overflow, read_point;
    Max86150_ReadData(0x00, 1, &read_buf[0]);
    Interrupt1 = read_buf[0];
    Max86150_ReadData(0x01, 1, &read_buf[0]);
    Interrupt2 = read_buf[0];
    Max86150_ReadData(0x04, 1, &read_buf[0]);
    write_point = read_buf[0];
    Max86150_ReadData(0x05, 1, &read_buf[0]);
    Overflow = read_buf[0];
    Max86150_ReadData(0x06, 1, &read_buf[0]);
    read_point = read_buf[0];

    buffer[count][0] = Interrupt1;
    buffer[count][1] = Interrupt2;
    buffer[count][2] = write_point;
    buffer[count][3] = Overflow;
    buffer[count][4] = read_point;

    if(count % 2 == 0){
        Max86150_ReadData(0x07, 3, &read_buf[0]);
    }
    count += 1;
}
```

#### Tflitemicro algo
- Open "tflitemicro_algo.cpp", put layers we have in model.
```cpp
	static tflite::MicroMutableOpResolver<8> micro_op_resolver;
	micro_op_resolver.AddMul();
	micro_op_resolver.AddAdd();
	micro_op_resolver.AddConv2D();
	micro_op_resolver.AddFullyConnected();
	micro_op_resolver.AddReshape();
	micro_op_resolver.AddMaxPool2D();
	micro_op_resolver.AddSoftmax();
	micro_op_resolver.AddRelu();
```
- Add tflitemicro algo.
```cpp
extern "C" int tflitemicro_algo_run(float * ECGData)
{
    int ercode = 0;

    float Nor = 1;
    for (int i = 0; i < kImageSize; i ++) 
      input->data.f[i] = ( *(ECGData + i * 2) ) / Nor;

    TfLiteStatus invoke_status = interpreter->Invoke();

    if(invoke_status != kTfLiteOk)
    {
	error_reporter->Report("invoke fail\n");
    }
	
    float* results_ptr = output->data.f;
    int result = std::distance(results_ptr, std::max_element(results_ptr, results_ptr + 2));
    ercode = result;
    return ercode;
}
```

#### Model setting
- Open model_settings.cpp and change label.
```cpp
const char kCategoryLabels[kCategoryCount] = { 'N', 'Y', };
```

#### GMA303KU part
- GMA303KU is same as "workshop/Synopsys_SDK_V22/Example_Project/Lab2_I2C_Accelerometer/src/synopsys_sdk_GMA303KU.c".
#### Main fuction
- Main fuction control all I/O and 3-axis accelerater.
```c
/**************************************************************************************************
    (C) COPYRIGHT, Himax Technologies, Inc. ALL RIGHTS RESERVED
    ------------------------------------------------------------------------
    File        : main.c
    Project     : WEI
    DATE        : 2018/10/01
    AUTHOR      : 902452
    BRIFE       : main function
    HISTORY     : Initial version - 2018/10/01 created by Will
    			: V1.0			  - 2018/11/13 support CLI
**************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "embARC.h"
#include "embARC_debug.h"
#include "board_config.h"
#include "arc_timer.h"
#include "hx_drv_spi_s.h"
#include "spi_slave_protocol.h"
#include "hardware_config.h"

#include "hx_drv_uart.h"

#include "tflitemicro_algo.h"
#include "model_settings.h"
#include "max86150.h"
#include "synopsys_i2c_oled1306.h"
#include "synopsys_sdk_GMA303KU.h"


#define MaxCounter 480

int16_t accel_x;
int16_t accel_y;
int16_t accel_z;
int16_t accel_t;

float ECG[MAX] = {0};


int main(void)
{    
    //UART 0 is already initialized with 115200bps
    tflitemicro_algo_init();
    uint8_t chip_id = GMA303KU_Init();
    board_delay_ms(100);

    OLED_Init();    
    OLED_Clear();
    OLED_SetCursor(0, 0);

    if(chip_id == 0xA3)
        DisplayResult('R');
    else
        OLED_DisplayString("GMA303KU_Init Error");
    board_delay_ms(10);

    while(1){
        uint16_t reg_04_data = GMA303KU_Get_Data(&accel_x, &accel_y, &accel_z, &accel_t);

        if(accel_z < -350 ){
            DisplayResult('N');
            break;
        }
        board_delay_ms(100);
    }

    int hr, min, yes = 0;

    InitUART();
    InitMax86150();

    while (1)
    {
        int index;
        GetECGloop(ECG);
        index = tflitemicro_algo_run(ECG);
        DisplayResult(kCategoryLabels[index]);
        if(kCategoryLabels[index] == 'Y'){
            yes += 1;
        }
        min += 1;
        uint16_t reg_04_data = GMA303KU_Get_Data(&accel_x, &accel_y, &accel_z, &accel_t);
        if(accel_z > -350 ){
            break;
        }
    }

    int AHI;
    if(min % 60 != 0){
        hr = (min / 60) + 1;
    }
    else{
        hr = (min / 60);
    }
    AHI = yes / hr;
    DisplayAHI(AHI);

	return 0;
}
```

## Final Project Introduction
- Project outward

<img width="450" alt="PC2" src="https://user-images.githubusercontent.com/87894572/177011186-f15199a4-1ed1-45a5-90d2-f44c48f13ead.png">

- Project feature
1. OCED1306 display "sleep" : ARC EM9D is in waiting state untill lay down go through sleeping status.

<img width="450" alt="PC2" src="https://user-images.githubusercontent.com/87894572/177011286-ce2bf7a2-0ddd-49c0-b9c9-1b2725b0595a.png">

2. OCED1306 display a heart with a "√" sign : Model dectect normal breathe per minutes.

<img width="450" alt="PC2" src="https://user-images.githubusercontent.com/87894572/177011666-f5e5d5a2-a0b0-46fc-8cfd-65b15a7eba66.png">

3. OCED1306 display a heart with a "X" sign : Model dectect sleep apnena per minutes.

<img width="450" alt="PC2" src="https://user-images.githubusercontent.com/87894572/177011627-4b93ba0d-799e-4b24-96f9-8e2e77bb78cb.png">

4. OCED1306 display "AI:XX" : User wake up and calculate apnena times in average hour.

<img width="450" alt="PC2" src="https://user-images.githubusercontent.com/85031209/181212625-a93794cc-7820-497b-9942-f437990c490f.jpg">

- Wearable device put on user

<img width="450" alt="PC2" src="https://user-images.githubusercontent.com/85031209/181212704-2f15329a-70ee-474a-b127-2f16c8eb198d.jpg">

- User lay on bed

<img width="450" alt="PC2" src="https://user-images.githubusercontent.com/87894572/177012511-0394668e-4695-45f6-a9cc-9d16ae9c3891.png">

- Demo

Youtube link:https://www.youtube.com/watch?v=K_oLKA99gfE
