# Project Covidentity
## Introduction
There are 500 million people infected with Covid-19 worldwide, and the current testing devices have several drawbacks, such as the inaccuracy of temperature measurement results and high cost of disposable quick screening kits.  Therefore, we hope to build a new detection method by packing ARC, sensors, and raspberry pi together.  With high accuracy and shorter predicting time, it collects ECG signals from fingers of both hands and uses neural network for analysis and prediction.  Finally, it can present the identity information of the subject and whether he or she has COVID-19 in the meantime on cloud.  We hope that this product can be used in various places and entrances to play a role in preventing the spread of the epidemic, while the reusable feature on our product can also reduce the cost of medical resources. 


## Hardware/Software Setup
### Hardware
- **BMD 101** is used as our ECG senser, data collected by the sensor will be transmitted to **raspberry pi** by **bluetooth**.  
- **raspberry pi** will preprocess the data for our model and send it to **ARC EM9D AIoT DK** by **Uart**.  
- **ARC EM9D AIoT DK** will generate prediction from the data and send the result back to **raspberry pi** by **Uart** and upload it to **cloud**  
- **Touch sensor** is used as a start button, connected to **raspberry pi** by **GPIO**
- **LED Bar** is used as a progress bar for data collection, connected to **raspberry pi** by **GPIO**


![hardware](https://github.com/heyibon/heyibon/blob/main/hardware.png)
### Software
#### COVID-19 Model
![hardware](https://github.com/heyibon/heyibon/blob/main/covid%20model.png)
#### Identification Model
![hardware](https://github.com/heyibon/heyibon/blob/main/identity%20model.png)

## User Manual
### Sensor
![user manual](https://github.com/heyibon/heyibon/blob/main/user%20manual.png)
### Cloud

result will be directly uploaded to cloud and be displayed on screen

<img src="https://github.com/heyibon/heyibon/blob/main/cloud1.png" width="600" height="300">

server side will receive user details including:
- ID
- Name
- Diagnose
- Time
- Location

<img src="https://github.com/heyibon/heyibon/blob/main/cloud2.png" width="600" height="250">
