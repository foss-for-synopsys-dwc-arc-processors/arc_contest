'''
This is a file for taking the model predict output from em9d
dataflow:
    1. Raspi take RGB picture ->
    2. Em9d take pic using its cam ->
    3. Em9d predict pic in food classification ->
    4. Em9d push the outcome to UART->
    5. Raspi recive the outcome

'''

import time
import serial
import io
import os
import sys

import struct
from PIL import Image
import codecs
import numpy


def connect_to_em9d(recr_port, send_port):
    """
    This is a function to set up the connection 
    between Raspi and Em9d
    """
    
    print("Connecting to the food detect module...\r")

    ## UART 1 is for taking the result from Em9d
    ## /dev/ttyUSB0 is UART1 in EM9D
    ser_recv = serial.Serial(recr_port, 115200)   
    
    ## UART 0 is for send the message to Em9d
    ## /dev/ttyUSB1 is UART0 in EM9D
    ser_send = serial.Serial(send_port, 115200)

    time.sleep(1)
    print("Connected!\r")
    
    return ser_recv, ser_send
    
def non_block_recieve(ser_recv,ser_send):
    """
    This function will loop until it gets result from Em9d
    """
    
    while (1):
        # read the data from ser_recieve serial port
        data_str = ser_recv.readline()
        time.sleep(0.1) 
        
        # if get data
        if data_str:
            print("get result from Em9d:", str(data_str, 'utf-8').rstrip('\n'))
            str_data = str(data_str, 'utf-8')
            predict_result = str_data[0]
  
            ser_send.write(1)
            
            return predict_result

def get_em9d_result():
    
    # establish serial port connection with Em9d
    ser_recv, ser_send = connect_to_em9d('/dev/ttyUSB0', '/dev/ttyUSB1')
    
    # Take image from Webcam
    # os.system("fswebcam -d /dev/video0 --no-banner -r 1080x720 /home/pi/Pictures/image_webcam.jpg")
    
    # config the image to fit in the model in Em9d
    image_path = 'image.jpg'  # Replace with the path to your RGB image
    image = Image.open(image_path)
    image = image.resize((64,64))
    image_width, image_height = image.size 
    
    # preprocess the image 
    print("turn image to bytes")
    image_arr = numpy.asarray(image)
    image_arr = image_arr.flatten('C')
    for i in range(len(image_arr)):
        if image_arr[i] == 0:
            # print("這裡有0")
            image_arr[i] = 1
            
    # send the image to Em9d 
    print("Sending image data...")
    image_arr = image_arr.tobytes()
    # print(image_arr)
    ser_send.write(image_arr)   
    print("Finish sending picure to uart")
    
    # get em9d model output
    print("Waiting for predict result")
    predict_result = non_block_recieve(ser_recv,ser_send)
    print("Finsish getting result from em9d")
    time.sleep(2)
    
    # close the serial port
    ser_send.close()
    ser_recv.close()
    
    # return the predict result and image path
    return predict_result, image_path
