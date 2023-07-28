import serial
import keyboard
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image
import cv2
import time

COM_PORT = 'COM4'
BAUD_RATES = 115200


img_height = 30 #150
img_width = 30 #150

img_list = []
data_str = ""
read_img = False
show_img = False

ser = serial.Serial(COM_PORT, BAUD_RATES)

cnt = 0

detect_list = ["not detected",
               "upper-left",
               "upper",
               "upper-right",
               "left",
               "detected",
               "right",
               "lower-left",
               "lower",
               "lower-right"]



while True:
    start = time.time()

    # Press [A]: adjust mode
    if keyboard.is_pressed("A") or keyboard.is_pressed("a"):
        key = "A"
        enc_key = key.encode()
        ser.write(enc_key)

    # Press [I]: inference mode
    if keyboard.is_pressed("I") or keyboard.is_pressed("i"):
        key = "I"
        enc_key = key.encode()
        ser.write(enc_key)

    if ser.in_waiting > 0:
        data = ser.readline().decode().strip()
        print(data)



        if '/' in data:
            detect = data.strip("/")
            detect_idx = int(detect)
            print("Detection: ", detect)
            print("Position: ", detect_list[detect_idx])


        if '<' in data:
            read_img = True
            img_list = []
            data_str = ""
            data_str += data
        

        if read_img: 
            data_str += data
            if '>' in data:
                img_list = data_str.strip("<").strip(">").strip(" ").split(",")
                # print("img_list: ", img_list)
                print("Image counter: ", cnt)
                cnt += 1
                read_img = False
                show_img = True

         
        if show_img:
            end = time.time()
            exe_time = end - start
            fps  = 1.0 / (end - start + 10e-4)
            print("FPS: %.2f" %fps)


            if len(img_list) == img_height*img_width:
                img_arr = np.array(img_list).astype(np.uint8).reshape((img_height, img_width))
                plt.clf()
                plt.imshow(img_arr, cmap='gray')
                plt.plot([14, 14], [0, 29], color="red")
                plt.plot([16, 16], [0, 29], color="red")
                plt.plot([0, 29], [14, 14], color="red")
                plt.plot([0, 29], [16, 16], color="red")
                plt.plot(15, 15, marker="o", color="magenta", markersize="10")
                plt.text(1, 2, "Synopsys ARC AIoT", color="magenta",  fontsize="14")
                plt.text(1, 4, "OASIS Titans", color="magenta",  fontsize="14")

                if detect_idx == 0:
                    plt.text(1, 6, "%s"%str(detect_list[detect_idx]), color="pink",  fontsize="14")
                elif detect_idx == 5:
                    plt.text(1, 6, "%s"%str(detect_list[detect_idx]), color="lightgreen",  fontsize="14")
                else:
                    plt.text(1, 6, "%s"%str(detect_list[detect_idx]), color="skyblue",  fontsize="14")

                plt.text(21, 2, "FPS: %.2f"%fps, color="orange",  fontsize="14")
                plt.axis('on')
                plt.show(block=False)
                plt.pause(0.01)
                show_img = False

serial.close()