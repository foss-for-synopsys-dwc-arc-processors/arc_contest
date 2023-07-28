**[2023 Synopsys ARC AIoT](https://contest.synopsys.com.tw/2023ARC?utm_source=synopsys.com&utm_mediu)**

# Presentation Video
[<img src="https://hackmd.io/_uploads/r1L5XyVtn.png" width="100%">](https://www.youtube.com/watch?v=OrEMm2kou74)




# Abstract
## Motivation
Many traditional factories are still using analog guages. However, to replace the new machine with digital readers will be large costs, and record or moniter guage data by human inspectors is a waste of time.
## Our Proposal

- Digtize the analog guage without replacing it
- Use ARC EM9D real-time monitor analog guage
- Reduce labor costs and enhanced management automation

![](https://hackmd.io/_uploads/rkYJIjWF2.png)

# Challenge
**1. Dataset Collection**

- Collect and annotate a sufficient number of data
- Different gauge styles and appearances
- Influence of lighting and noise

![](https://hackmd.io/_uploads/r1YE8ibF2.png =80%x)

**2. Model Sellection**

* Extraction and recognition of numbers -> Large-scale detection and Classification model
* Angle prediction for gauge pointer -> Regression or Multi-class Classification

**3. Computation and Storage Limitations**

* Compress the model to deploy on the device
* Maintain the model performance with limited model size

**4. Traditional Computer Vision Methods Are Sensitive**

* Variations in lighting, background clutter, particular types of guages

**5. Exist Dataset Lacks Realism**

* Inaccurate background representation
* Without considering image distortion
# Innovation
## Analog Guage Image Data Generator
- Gauge Images and Videos: Pressure Gauge Dataset(Source: Kaggle)
- Backgroud Images: Places Dataset-engine_room(Source: MIT)

![](https://hackmd.io/_uploads/rJxqUsZt2.png =110%x)

## Data Augmentation
**- Random Enhancement**
* Saturation
* Brightness
* Contrast
* Sharpness

**- Random Noise**
* Gaussian Noise

**- Random Blur**
* Gaussian Blur

    ![](https://hackmd.io/_uploads/ryF0LsZF2.png)

## Fisheye Transform
- Simulate the effects of image distortion
    1. Normalize pixels in the interval of [-1, 1]
    2. Cartesian to polar: ![](https://hackmd.io/_uploads/rynLPoZFn.png =20%x)

    3. Distortion: ![](https://hackmd.io/_uploads/r1Qx_ibK3.png =40%x) ,where *d* is the distortion constant
    4. Polar to Cartesian: ![](https://hackmd.io/_uploads/S1r6ujZYn.png =40%x)

    ![](https://hackmd.io/_uploads/S1XMFobYn.png =70%x)


    ![](https://hackmd.io/_uploads/S1e4KoWth.png =70%x)

## Fisheye Transform to AoSTM VGA Camera

![](https://hackmd.io/_uploads/SkwUFiZYh.png)



## Analog Guage Calibration
### To determine the number of output classes
- Limitation of int8 quatization from 0~255, so model is unable to predict angles within 0~360 degrees
- Numbers of the classes impact the size of the output FC layer

### Trade-off Between Accuarcy, Precision & Model Size

![](https://hackmd.io/_uploads/B1-2zhXFh.png =80%x)


![](https://hackmd.io/_uploads/H1EGqjZK2.png =30%x) ![](https://hackmd.io/_uploads/H1Ue5ibFh.png =60%x)



## Knowledge Distillation
### Compress Model Through Knowledge Distillation
- Matching prediction probability between teacher and student model

![](https://hackmd.io/_uploads/rkLHco-Fh.png =90%x)

# Design and Reliability 
## System Architecture

![](https://hackmd.io/_uploads/BkaKGIftn.png)

## Analog Gauge Reader
* **MobileNet V2**

    ![](https://hackmd.io/_uploads/Sy02m2mt2.png =90%x)

    ![](https://hackmd.io/_uploads/ByzCQ3QF3.png =40%x) ![](https://hackmd.io/_uploads/rJfk4n7t3.png =50%x)

* **Gauge Reader Converter**

    ![](https://hackmd.io/_uploads/ry86V37F3.png =70%x)

## Post-Training Quantization
* **Reduce Memory Storage & Computational Resources**
    * **Model Weights:** fp32 → int8
    * **Steps:** 
        1. Train fp32 TF model (model.pb)
        2. Evaluation fp32 model on GPU
        3. Quantize the model weight
        4. Convert the fp32 model to int8 TFLM model (model.tflite)
        5. Evaluation int8 model on GPU
        6. Convert the TFLM model to C file (model.h)
        7. Inference on ARC EM9D
        
    ![](https://hackmd.io/_uploads/SJ6mI37Kh.png =60%x)

![](https://hackmd.io/_uploads/rkQSUhQth.png)


# Hardware and Software setup
## Software version
    python=3.8.0
    tensorflow=2.5.0
## Hardware specification
    GPU: NVIDIA GeForce GTX 2080 Ti
## Create Enviroment
    conda create -n 2023ARC python=3.8

## Activate Enviroment
    conda activate 2023ARC

## Packages Installation
    conda install jupyter notebook
    pip install tensorflow-gpu==2.5.0
    conda install numpy==1.23.4
    conda install matplotlib
    conda install pandas
    conda install -c anaconda scikit-learn
    conda install -c conda-forge keras
    pip install opencv-python
    pip install tqdm

# User-Manual
Connect ARC EM9D AIoT DK with Wemos D1 R32

    UART0 RX header connect to GPIO16
    UART0 TX header connect to GPIO17
    GND to GND
# Demo Video
* Demo 1: https://www.youtube.com/watch?v=Uo0q9bvPye8

<iframe width="560" height="315" src="https://www.youtube.com/embed/Uo0q9bvPye8" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>
 
 ---
* Demo 2: https://www.youtube.com/watch?v=cPMuLgIlY1I
 
<iframe width="560" height="315" src="https://www.youtube.com/embed/cPMuLgIlY1I" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>

---
* Demo 3: https://www.youtube.com/watch?v=dGtSQWrOaJU

<iframe width="560" height="315" src="https://www.youtube.com/embed/dGtSQWrOaJU" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>

---
* Demo 4: https://www.youtube.com/watch?v=Z6F6YlwSd7A

<iframe width="560" height="315" src="https://www.youtube.com/embed/Z6F6YlwSd7A" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>

---
* Demo 5: https://www.youtube.com/watch?v=5TH9fBItiWc

<iframe width="560" height="315" src="https://www.youtube.com/embed/5TH9fBItiWc" title="2023 Synopsys ARC AIoT Design Contest - [OASIS Titan] Analog Gauge Reader Demo" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>



# Overall Summary
## Real-time Analog Gauge Reader
**1. Data Generator Generate**
* Automatically generates and labels images to create a training dataset
* Suitable for the real-world scenario

**2. Analog Gauge Calibration**
* Decide the number of categories
* Trade-offs between model size and accuracy

**3. Model Compression**
* Reduce the size of MobileNetV2
* Using int8 quantization to compress the model size

**4. UI for Recording and Displaying Analog Gauge**

![](https://hackmd.io/_uploads/BJ75XAXtn.png =70%x)
