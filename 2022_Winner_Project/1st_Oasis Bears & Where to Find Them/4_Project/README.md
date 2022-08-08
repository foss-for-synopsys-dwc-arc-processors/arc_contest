# **[2022 Synopsys ARC AIoT Design Contest](https://contest.synopsys.com.tw/2022ARC)**

<!-- <a href="https://contest.synopsys.com.tw/2022ARC
" target="_blank"><img src="https://user-images.githubusercontent.com/96005167/181190562-62bdda9f-2405-4c3a-b0f7-1f53a0abd062.png"
alt="Link is failed!" width="300" border="10" /></a> -->
<!-- # **Contactless Control Panel** -->

# Team Info.
### NCTU Oasis Lab (Machine Learning Group)
<!-- ![](https://user-images.githubusercontent.com/96005167/181191742-5188627a-c551-4014-901f-e627fd9b6ce2.png) -->
<a href="https://sites.google.com/a/g2.nctu.edu.tw/oasis_317a/
" target="_blank"><img src="https://user-images.githubusercontent.com/96005167/181191742-5188627a-c551-4014-901f-e627fd9b6ce2.png"
alt="Link is failed!" width="400" border="10" /></a>

### **非接觸式控制面板 - Contactless Control Panel**
### 綠洲熊與他們的窩
### Advisor: 張錫嘉 教授
### Members: 陳冠瑋、鄭紹文、曹家輔、李家毓
### Introduction:
We are students from the Oasis Lab of National Chiao Tung University (NCTU).

The research field of the lab covers hardware implementation of ML, ECC, and Security. We hope that the ideals and works will be turned into reality through this competition, and we will try our best to complete the project worth looking forward to!


# <font color="#4E2683">Outline</font>
### [1. Introduction](#introduction-1)
### [2. Difficulties & Innovation](#difficulties--innovation)
### [3. Design & Implementation](#design--implementation)
### [4. Demo & Result](#demo--result)
### [5. Conclusion](#conclusion)

 
# Introduction
<!-- https://user-images.githubusercontent.com/96005167/181204805-ef69177c-7cb1-4f42-ae35-33eff0ccf267.mp4 -->

### <font color="#4E2683">【Motivation】</font>
* Smart building
* Smart security
* Prevent epidemic

### <font color="#4E2683">【Solution】</font>
We propose a contactless control panel combined with face authentication, which can send control signals and verify the user’s identity without touching the panel ! 

* Using “Siamese Network” to train a face authentication model with a small number of examples, and deploy the model on ARC EM9D AIoT DK with TensorFlow Lite.
* Using Mediapipe Hand and OpenCV to implement a contactless virtual control panel through gesture recognition on NVIDIA Jetson Nano.

### <font color="#4E2683">【Scenario 1】: </font>Virtual elevator button panel + Visitor verification
*  Control the elevator with gesture recognition
*  Visitor verification
*  Virtual elevator control panel

![](https://user-images.githubusercontent.com/96005167/181206306-792f0f50-0c1e-4d1d-8ec2-fed1f45ca1ff.gif)

### <font color="#4E2683">【Scenario 2】: </font>Smart door lock 
*  Identity verification
*  Stranger Record
*  Virtual touchless door lock

### <font color="#4E2683">【Scenario 3】: </font>Contactless ticket machine screen
* Record the usage time of different users
* Combined with automatic recommendation system
* Avoid touching public touchscreens


# Difficulties & Innovation
### <font color="#4E2683">【Difficulties】</font>
#### - Model size
* Available memory size < 2MB
* Auto-Encoder: Dimension Reduction
* Post training quantization: fp32 → int8

#### - Input images pairs for the Siamese network
* Siamese network needs to compare two images at the same time
![image](https://user-images.githubusercontent.com/96005167/181212761-e70de490-c7af-4a76-9625-3cca4ffae1f4.png)

#### - Transmission time
* Validation data transmission
* Save images for comparison in the system memory
![image](https://user-images.githubusercontent.com/96005167/181213995-b7745e55-ff11-4a5d-a780-9d21a1a75de3.png)


#### - MLI Library does not support some Ops
* In the original paper of Siamese Network, the sigmoid function is used as the activation function of the last layer
* However, MLI Library does not support the sigmoid function as activation function
* When we tried to use tanh as activation function with MSE as the loss function, we found that tanh did not support it either
* When implement the L1 distance layer, int8 abs() op does not support too




### <font color="#4E2683">【Innovation】</font>
#### - Siamese Network Architecture on TinyML
 - Discriminate the similarity between two different input
 - Small number of samples are needed

#### - Small face verification model
 - Low power: Used in unreliable power supply scenarios 
 - Less computing & memory resources: Faster inference, faster deployment

#### - Input dimension reduction 
 - Auto-Encoder: Reduce the dimension of the input features
 - Solve the bottleneck of inference time
 - Deploy a more efficient model

####  - Combining control panel with computer vision
 - In the future, we hope that it can combined with AR glasses and widely used in the metaverse.


# Design & Implementation
### <font color="#4E2683">【Hardware Architecture】 </font>


![image](https://user-images.githubusercontent.com/96005167/181218301-72d60ad7-74b0-492b-9ca0-9ec74bff7b87.png)

### <font color="#4E2683">【ARC EM9D AIoT DK】 </font>
* Task: Face verification 
* Model: Siamese Network Architecture


### <font color="#4E2683">【NVIDIA Jetson Nano】</font>
* Task: Hand pose Estimation, gesture recognition, and System control
* Model: MediaPipe Hand


### <font color="#4E2683">【Siamese Network】</font>
![image](https://user-images.githubusercontent.com/96005167/181220508-c1eff71a-083a-47f6-b595-6f56e73e2a1c.png)

*  Siamese network inspired by “Siamese twins” which has a unique architecture
*  Classify if the two inputs are the same or different (learn the similarity)
*  Take two different inputs passed through two similar subnetworks 
*  The two subnetworks have the same architecture, parameters, and weights
*  Convert the classification problem to a similarity problem
![image](https://user-images.githubusercontent.com/96005167/181219965-f150b460-23c9-411d-9a29-7fdb2f0d623c.png)
![image](https://user-images.githubusercontent.com/96005167/181219585-53201978-c137-4b31-b75f-0ced7139e13f.png)



### <font color="#4E2683">【Why choose “Siamese Network" ?】</font>
*  Humans exhibits a strong ability to acquire and recognize new patterns
*  A few-shot classification model


### <font color="#4E2683">【Data Preprocessing】</font>
#### -Resize: 100x100x1
#### -Data augmentation
#### -Datase
* Anchor: 600
* Positive: 600
* Negative: 600 ([Label Face in the Wild Dataset](http://vis-www.cs.umass.edu/lfw/) + self collect)


![image](https://user-images.githubusercontent.com/96005167/181220830-c06e1ef9-5e4a-4db1-9705-d22bdd2bf746.png)

### <font color="#4E2683">【AutoEncoder】 </font>
#### -Dimension Reduction
* Compress input image into latent with Encoder
* Use latent to recover image with  Decoder

![image](https://user-images.githubusercontent.com/96005167/181225748-09712221-9d2b-4d83-a2d1-e4f748716809.png)

![image](https://user-images.githubusercontent.com/96005167/181225863-1788c1fc-b150-43f6-b743-519706f4aedd.png)


#### -Method
* Use VGG-16 as an architecture baseline

![image](https://user-images.githubusercontent.com/96005167/181225964-6c96e330-00a5-48f0-b783-868d76435672.png)

![image](https://user-images.githubusercontent.com/96005167/181225991-6efd1911-82ba-4731-b820-43cd35df60dc.png)


### <font color="#4E2683">【AutoEncoder + Siamese Network】 </font>

![image](https://user-images.githubusercontent.com/96005167/181227053-05a975c7-411f-4452-95fa-de34d910a444.png)


### <font color="#4E2683">【Virtual Button Panel】 </font>
* Hardware: NVIDIA Jetson Nano
* Hand Tracking: Google MediaPipe Hand tracking module
* Virtual Button: OpenCV
* Click the virtual panel through the gesture
* Index Finger: Select the virtual button
* Index Finger + Middle Finger: Click the virtual button

![image](https://user-images.githubusercontent.com/96005167/181224910-b5137257-aad7-439c-b21d-49608ed8292f.png)


* **IDLE:**
 1. In the IDLE state, the system will stop performing gesture recognition, which can reduce the power consumption. 
 2. The HC-SR04 are used as the trigger of the system. 
 3. If face or an object is detected, the system will enter the RECOGNIZING state. 

* **RECOGNIZING:**
 1. In the RECOGNIZING state, the system will recognize the user’s gesture and communicate with the PC, and will continue to monitor the signal from the trigger.
 2. If it is not triggered for 15 seconds, the system will enter the IDLE state.
 3. If there is a trigger signal, it will reset the timmer.


![](https://i.imgur.com/p1pRgkU.png)


### <font color="#4E2683"> 【MediaPipe Hand Landmarks】</font>

* The palm detector model is used to find the region of the palm from the image.
* The hand landmarks model is used to mark the 3D key-points in the region of interest.
* Measure the distance between the index finger and the middle finger to achieve the click operation to select the buttons of virtual panel.
![](https://i.imgur.com/MVDNqC6.png)


### <font color="#4E2683">【Simulation Animation】 </font>
* Use the Pygame library on the server to simulate the situation of the elevator. 
    * Going up and down
    * Opening and closing
* Send the result signal to the server through Wi-Fi to control the elevator simulation animation.
* In addition, we also consider the problem of elevator scheduling, so that the elevator can update the input floor in real time and can input multiple floors at the same time.
![image](https://user-images.githubusercontent.com/96005167/181224106-4036d2fe-6567-43df-a8b3-c59f2d867e22.png)


# Demo & Result
### <font color="#4E2683">【Inferece on PC】 </font>
![image](https://user-images.githubusercontent.com/96005167/181228606-2d5c1c3b-1ebb-4a03-8392-1f2d047b3c8f.png)




Demo1: https://www.youtube.com/watch?v=hRp12IpJmxE&ab_channel=ChenBaker

[<img src="https://user-images.githubusercontent.com/96005167/181232933-6f705d0c-0a00-4d50-8755-cf4605f385b1.png" width="50%">](https://www.youtube.com/watch?v=hRp12IpJmxE&ab_channel=ChenBaker)



### <font color="#4E2683">【Inferece on ARC EM9D】 </font>
![image](https://user-images.githubusercontent.com/96005167/181228097-327b7039-6d2d-4c30-bf21-f913e5a602b6.png)
![image](https://user-images.githubusercontent.com/96005167/181227574-f1c76aeb-5f58-4713-a813-f43b92800bb1.png)



### <font color="#4E2683">【Siamese Network (Orig.)】</font>
![](https://i.imgur.com/TugU36L.png)


### <font color="#4E2683">【Performance】</font>
![image](https://user-images.githubusercontent.com/96005167/181235631-be8130a1-a5df-4a34-9253-293809b35323.png)


### <font color="#4E2683">【Demo】</font>
Demo2: https://www.youtube.com/watch?v=tXEByFnG8Jw&t=18s&ab_channel=ChenBaker

[<img src="https://user-images.githubusercontent.com/96005167/181236293-54d89b35-f6c5-4fa7-940d-e8b61bb5dcee.png" width="50%">](https://www.youtube.com/watch?v=tXEByFnG8Jw&t=18s&ab_channel=ChenBaker)

Demo3: https://youtu.be/AG2H8PWU4Uw
[<img src="hhttps://user-images.githubusercontent.com/96005167/181241614-dd8d2346-7eb8-4aa8-8fc1-1e1fb4faf82f.png" width="50%">](https://youtu.be/AG2H8PWU4Uw)


# Conclusion
### <font color="#4E2683">【Hope to add…】 </font>
* Recording the time when stranger enter the building.
* Voice control 

### <font color="#4E2683">【Hope it can be applied in the following scenarios】</font>
* Smart door lock
* Smart food ordering machine
* Smart automatic ticketing system
* ATM

### <font color="#4E2683">【Our Experience !】</font>

![image](https://user-images.githubusercontent.com/96005167/181235945-a1873c05-be95-4487-a3c0-65105bbe297d.png)


我們實驗室ML研究團隊自2018年起即開始參加Synopsys ARC AIoT競賽，期間有成功亦有失敗，從佳作到今年的冠軍，一路走來，每一次的努力都是不可多得的養分。今年結合大AI時代以及全球疫情，實現出非接觸式控制面板系統，這種邊緣端裝置對於功耗和晶片面積有不小的要求，我們利用新思科技的ARC EM9D處理器來實現高效臉部識別，結合後端處理來實現系統設計，過程中遇到不少問題，一步一步慢慢的利用MLI軟體庫尋找有效的解決辦法，最後的成品很開心能夠獲得評審們的賞識，希望日後Synopsys ARC AIoT能提供更強大的platform，來讓大家激盪且實現出更有趣的設計!


Our ML research team at National Chiao Tung University has been participating in the Synopsys ARC AIoT competition since 2018. There were successes and failures during this period. From the best work to this year's champion, every effort is a rare nutrient along the way. This year, we implement the contactless virtual control panel which was inspired by the big AI era and the global pandemic. An edge device like this idea usually concerns the power consumption and the chip area. We use the Synopsys ARC EM9D processor to fulfill efficient face verification and combine it with the backend processing to accomplish the whole design. We felt like being up against the wall and felt frustrated in these several months, but as the saying goes “When the going gets tough, the tough get going.”. By using the MLI to find suitable solutions, it’s our pleasure that our project can favor by the judges. Hope that Synopsys can provide the more powerful processor for the contestants to design more interesting and efficient implementation.
