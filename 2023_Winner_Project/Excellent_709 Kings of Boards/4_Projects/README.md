# FoodieFortress
<div align=center><img src=https://i.imgur.com/xT1Zhr4.png width="350px";></div>
## Introduction

我們使用 ARC EM9D AIoT DK開發板進行食品的辨識，設計出 共享智慧冰箱 — FoodieFortress。不但可以記錄冰箱中每個食物的所有者，並在每次有人放入/拿取食品時，記錄時間、接觸對象以及食品資訊。

FoodieFortress可以防止使用者的誤吃別人的食物，並在食品於冰箱存放一段時間後，主動通知食品的擁有者，避免食品的腐敗。

## HW/SW Setup

### Hardware

- ARC EM9D AIoT DK 開發板 
- 光學指紋感應器：AS608 Optical Fingerprint Reader Sensor Module
- 食品影像辨識鏡頭：Logitech c310 webcam
- Raspberry Pi 4

![](https://i.imgur.com/UkvrO4P.jpg)

### Software

### System Flow

![](https://i.imgur.com/0hi8AEf.png)

### Model
**Training Dataset**: [Fruits360](https://www.kaggle.com/datasets/moltean/fruits)
**Model**: Custom-ResNet

![](https://i.imgur.com/cKqyUOm.jpg)

### Line Bot

FridgeLineBot為我們的User Interface，使用者可使用FridgeLineBot與系統互動。
使用系統前需與FridgeLineBot成為好友。
該Bot會記錄使用者的使用紀錄，包括使用者註冊、放入、取出的紀錄等，並記錄於一SQLite Database中。

## User Manual

### 註冊

1. 於FridgeLineBot對話框中按下「使用者註冊」，並將手指放於冰箱的指紋辨識器上。
2. 靜止不動，等待Bot回應。
3. 於Line上確認註冊成功的訊息。

### 放入食物

1. 將食物放進冰箱。
2. 將冰箱上的兩段式開關往**上**撥動，並將手指放上指紋辨識器。
3. 確認冰箱上的紅色LED是否閃爍，該燈閃爍表示系統正在辨識食物中。
4. 確認冰箱上的綠色LED是否閃爍，該燈閃爍表示系統已成功辨識食物，並將該次操作記錄於資料庫中。

### 取出食物

1. 將要取出的食物放在冰箱的鏡頭前。
2. 將冰箱上的兩段式開關往**下**撥動，並將手指放上指紋辨識器。
3. 確認冰箱上的紅色LED是否閃爍，該燈閃爍表示系統正在辨識食物中。
4. 確認冰箱上的綠色LED是否閃爍，該燈閃爍表示系統已成功辨識食物，並將該次操作記錄於資料庫中。

### 查看冰箱內食物 / 查看即期食品

1. 於FridgeLineBot對話框中按下「查看我的食物」或「查看即期食品」。
2. Bot會根據按下的按鈕回應相對應的資訊。

<div align=center><img src="https://i.imgur.com/GoXH2x0.png" width="300px"></div>