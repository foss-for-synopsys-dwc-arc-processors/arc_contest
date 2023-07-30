# An intelligent steering wheel reminded by driver's heart
## Introduction
We have developed an intelligent steering wheel system that aims to enhance driver safety and alertness. This system incorporates multiple functionalities to sequentially detect driver behavior, including improper hand placement on the steering wheel, driver fatigue, and drunk driving.

Here are the example images showcasing the functionalities of our intelligent steering wheel system:

<p float="left">
  <img src="https://github.com/hsh0615/picture/blob/main/%E5%9C%96%E7%89%878.png" alt="Improper Hand Placement" width="290" />
  <img src="https://github.com/hsh0615/picture/blob/main/%E5%9C%96%E7%89%877.png" alt="Driver Fatigue Detection" width="290" /> 
  <img src="https://github.com/hsh0615/picture/blob/main/%E5%9C%96%E7%89%879.png" alt="Drunk Driving Detection" width="290" />
</p>

## HW/SW Setup
### 【Hardware Requirements】
- **ARC EM9D board** (IoT Applications Platform)
- **MAX30102** (PPG Sensor, Physiological Signal Detection)
- **GREEN/RED LED** (Status Indicator)
- **Vibration Motor** (Warning Vibration Reminder)
- **MQ3** (Alcohol detector sensor)
- **Neo6m** (GPS tracker) for cars report a vehicle's location
- **TTGO** (WiFi module) for vehicular communication function
![hardware](https://github.com/hsh0615/picture/blob/main/%E5%9C%96%E7%89%875.png)
### 【Software】
- **PPG SQI Model**
![image](https://github.com/hsh0615/picture/blob/main/SQI-good-bad.png)
![image](https://github.com/hsh0615/picture/blob/main/accuracy-sqi.png)
- **Yawn detection Model**
![image](https://github.com/hsh0615/picture/blob/main/Yawn.png)
![image](https://github.com/hsh0615/picture/blob/main/accuracy-yawn.png)

## User manual
### 【Setup on Car】
![硬體](https://github.com/hsh0615/picture/blob/main/%E7%A1%AC%E9%AB%94.png) ![汽車](https://github.com/hsh0615/picture/blob/main/car-resize.png)
### 1. Hands Off the Wheel Testing
![image](https://github.com/hsh0615/picture/blob/main/hand-off.png)
### 2. Driver Yawn Testing
![image](https://github.com/hsh0615/picture/blob/main/Driver-Yawn.png)
### 3. Driver Drunkenness
![image](https://github.com/hsh0615/picture/blob/main/Drunk.png)
### 4. Cloud Testing
- When the Safety Steering Wheel detects potential signs of drunk driving, it will trigger the cloud testing process.
![Hardware](https://github.com/hsh0615/picture/blob/main/python-cloud-gps.png)

### Youtube link
- https://www.youtube.com/watch?v=9_kwLSGJuDw
