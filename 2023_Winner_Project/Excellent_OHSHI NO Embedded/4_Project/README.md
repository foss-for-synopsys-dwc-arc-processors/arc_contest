# IOT-based emergency ambulance detection system
## Introduction
Based on the START triage method and inspired by the emergency response documentary of the Puyuma train derailment accident, we have conceived an emergency medical triage system based on machine learning and the Internet of Things.
During large-scale accidents, a single device can classify and assess injuries for individual patients while simultaneously measuring six vital signs in real-time, including respiration, heartbeat, blood oxygen level, body temperature, diastolic pressure, and systolic pressure. This device addresses the limitations of traditional triage tools such as START. Additionally, it is equipped with an embedded system that allows for monitoring during emergencies and provides immediate alerts through voice reminders. Moreover, using Bluetooth technology, this device can transmit the patients' vital signs and location information to the disaster response center, enabling rescue personnel to promptly understand the patients' conditions.

Here are the example images showcasing the functionalities of our intelligent steering wheel system:



<div style="display: flex; justify-content: center; align-items: flex-start;">
  <img src="https://github.com/tw13256/IOT-based-emergency-ambulance-detection-system/blob/main/%E6%89%8B%E9%83%A8.jpg" alt="Hand" width="150" style="margin: 0 10px;">
  <img src="https://github.com/tw13256/IOT-based-emergency-ambulance-detection-system/blob/main/%E7%AE%AD%E9%A0%AD.jpeg" alt="Driver Fatigue Detection" width="200" style="margin: 0 10px;">
  <img src="https://github.com/tw13256/IOT-based-emergency-ambulance-detection-system/blob/main/%E7%B6%A0%E8%89%B2.PNG" alt="Drunk Driving Detection" width="120" style="margin: 0 10px;">
</div>

## HW/SW Setup
### 【Hardware Requirements】
- **     ARC EM9D       **(IoT Applications Platform)
- **      Buttons		**(Selecting a Level of Detection)
- **   Finger switch    **(Locking Detection Level)
- **      MAX86150      **(PPG Sensor 			  , Physiological Signal Detection)
- **       TMP117		**(Body Temperature Sensor, Physiological Signal Detection)
- **       ST7735		**(Full Color LCD Monitor , Displays injury level and vital signs)
- ** 	  SD178BMI      **(Voice Module			  , Accidental Voice Alert)
- **      PCA9672		**(Expansion I/O		  ,Connections to ST7735 DC, Pushbutton, Thumb Switch)
- **       HC-05		**(BLE     				  ,Transmit vital signs and location to on-site disaster centers)
![hardware](https://github.com/tw13256/IOT-based-emergency-ambulance-detection-system/blob/main/%E6%9E%B6%E6%A7%8B.PNG)
### 【Software】
- **Blood Pressure Estimation Algorithm**
<div style="display: flex; justify-content: center; align-items: flex-start;">
  <img src="https://github.com/tw13256/IOT-based-emergency-ambulance-detection-system/blob/main/%E8%A1%80%E5%A3%933.PNG" alt="bp1" width="450" style="margin: 0 10px;">
  <img src="https://github.com/tw13256/IOT-based-emergency-ambulance-detection-system/blob/main/%E7%AE%AD%E9%A0%AD.jpeg" alt="bp2" width="180" style="margin: 0 10px;">
  <img src="https://github.com/tw13256/IOT-based-emergency-ambulance-detection-system/blob/main/%E8%A1%80%E5%A3%93.PNG" alt="bp3" width="370" style="margin: 0 10px;">
</div>

<div style="display: flex; justify-content: center; align-items: flex-start;">
  <img src="https://github.com/tw13256/IOT-based-emergency-ambulance-detection-system/blob/main/%E8%A1%80%E5%A3%932.PNG" alt="bp1" width="550" style="margin: 0 10px;">
  <img src="https://github.com/tw13256/IOT-based-emergency-ambulance-detection-system/blob/main/%E6%A9%9F%E5%99%A8%E5%AD%B8%E7%BF%92.PNG" alt="bp2" width="450" style="margin: 0 10px;">
 
</div>

- **Heart Rate Algorithm**
 
![image](https://github.com/tw13256/IOT-based-emergency-ambulance-detection-system/blob/main/%E5%BF%83%E8%B7%B31.PNG)
![image](https://github.com/tw13256/IOT-based-emergency-ambulance-detection-system/blob/main/%E5%BF%83%E8%B7%B32.PNG)

- **Blood Oxygen(SPO2) Algorithm**
 
![image](https://github.com/tw13256/IOT-based-emergency-ambulance-detection-system/blob/main/%E8%A1%80%E6%B0%A71.PNG)
![image](https://github.com/tw13256/IOT-based-emergency-ambulance-detection-system/blob/main/%E8%A1%80%E6%B0%A72.PNG)

## User manual
### 1. Press the button to change the emergency level of START.
![硬體](https://github.com/tw13256/IOT-based-emergency-ambulance-detection-system/blob/main/step1.PNG)
### 2.Toggle the switch to lock the emergency level and put 2 fingers on the sensor to get Physiological signals.
### The signals will be updated every 5 seconds.
![image](https://github.com/tw13256/IOT-based-emergency-ambulance-detection-system/blob/main/step2.jpg)

### Youtube link
- https://youtu.be/23Wy5UuA4ng
