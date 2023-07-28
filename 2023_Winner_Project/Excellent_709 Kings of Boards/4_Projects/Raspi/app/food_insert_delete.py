from atexit import register
import os, time, sys
from argparse import ArgumentParser
from utils import enroll, my_food, expiration, check_fingerID_exist, register_fingerID, check_login, get_userIDby_lineid, insert_food
from utils import fake_api, take_food, food_detail, get_userID
from yolo_predict import take_pic
from google_api import exp_time_api
from get_em9d import get_em9d_result
from user_finger_print import user_loggin
from flask import Flask, request, abort
import sqlite3
import pika

from ultralytics import YOLO
from numpy import argmax
import torch

import threading
from multiprocessing import Process, freeze_support
import RPi.GPIO as GPIO

LED_CHIP=0
BUTTON=22
LED_GREEN_OFFSET=21
LED_RED_OFFSET = 20

def flash(pin_num):
    while(1):
        print("blink")
        GPIO.output(pin_num, GPIO.HIGH)
        time.sleep(0.4)
        GPIO.output(pin_num, GPIO.LOW) 
        time.sleep(0.4)

def light_up(pin_num):
    GPIO.output(pin_num, GPIO.HIGH)
    time.sleep(3)
    GPIO.output(pin_num, GPIO.LOW) 

if __name__ == '__main__':
    time.sleep(1)
    print("-----------------------------------------------------")
    print("Welcome to FoodieFort!\nHere we keep your food fresh and safe:)")
    print("-----------------------------------------------------")
    time.sleep(1)

    GPIO.setmode(GPIO.BCM)
    GPIO.setup(LED_GREEN_OFFSET, GPIO.OUT, initial=GPIO.LOW)
    GPIO.setup(LED_RED_OFFSET, GPIO.OUT, initial=GPIO.LOW)
    
    # GPIO.output(LED_GREEN_OFFSET, GPIO.HIGH)
    GPIO.setup(BUTTON, GPIO.IN)
    
    while(1):
        fingerID, ifExist = user_loggin(False)
        user_id = get_userID(fingerID)
        if (ifExist and user_id != -1):

            # 閃燈前關閉綠燈
            GPIO.output(LED_GREEN_OFFSET, GPIO.LOW)

            # 閃燈開始
            p = Process(target=flash , args=(LED_RED_OFFSET,))

            # 執行該子執行緒
            p.start()

            time.sleep(2)
            
            # food_type = take_pic()
            food_type_synoposys = get_em9d_result()

            # 閃燈結束
            p.terminate()

            # 關閉紅燈
            GPIO.output(LED_RED_OFFSET, GPIO.LOW) 

            # 開關 看insert 或 取出
            flag_insert_delete = GPIO.input(BUTTON)

            # 從image位置抓file辨識文字 (google_api.py)
            # 放入食物
            if(flag_insert_delete):
                exp_time = exp_time_api()
                if( insert_food(food_type, user_id, exp_time) ):
                    # 取出成功
                    # 亮燈
                    print("insert succ")
                    p = Process(target=light_up , args=(LED_GREEN_OFFSET,))
                else:
                    # 取出失敗
                    # not your food
                    # 亮燈
                    print("insert fail")
                    p = Process(target=light_up , args=(LED_RED_OFFSET,))
            # 取出食物 
            else:
                if( take_food(food_type, user_id) ):
                    # 取出成功
                    # 亮燈
                    print("take succ")
                    p = Process(target=light_up , args=(LED_GREEN_OFFSET,))
                else:
                    # 取出失敗
                    # not your food
                    # 亮燈
                    print("take fail")
                    p = Process(target=light_up , args=(LED_RED_OFFSET,))
