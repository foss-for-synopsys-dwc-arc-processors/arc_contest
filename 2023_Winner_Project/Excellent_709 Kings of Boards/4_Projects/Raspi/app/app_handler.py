# -*- coding: utf-8 -*- #此段可以中文註解，否則會執行錯誤
# from asyncio.windows_events import NULL
from atexit import register
from mimetypes import init
import os, time, sys
from argparse import ArgumentParser
from utils import enroll, my_food, expiration, check_fingerID_exist, register_fingerID, check_login, get_userIDby_lineid, insert_food
from utils import fake_api, take_food, food_detail, get_userID
from yolo_predict import take_pic
from google_api import exp_time_api

from user_finger_print import user_loggin, regist_finger
from flask import Flask, request, abort
import sqlite3
import pika

from ultralytics import YOLO
from numpy import argmax
import torch

import RPi.GPIO as GPIO

from get_em9d import get_em9d_result

import threading
from multiprocessing import Process

LED_CHIP = 0
BUTTON_OFFSET = 22
LED_GREEN_OFFSET = 21
LED_RED_OFFSET = 20

def led_light_up(pin_num):
    GPIO.output(pin_num, GPIO.HIGH)
    time.sleep(3)
    GPIO.output(pin_num, GPIO.LOW) 

def led_blink(pin_num):
    while(1):
        print("blink")
        GPIO.output(pin_num, GPIO.HIGH)
        time.sleep(0.4)
        GPIO.output(pin_num, GPIO.LOW) 
        time.sleep(0.4)

def init_led():
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(LED_GREEN_OFFSET, GPIO.OUT, initial=GPIO.LOW)
    GPIO.setup(LED_RED_OFFSET, GPIO.OUT, initial=GPIO.LOW)
    GPIO.setup(BUTTON_OFFSET, GPIO.IN)

def finger_detect():
    time.sleep(1)
    print("-----------------------------------------------------")
    print("Welcome to FoodieFort!\nHere we keep your food fresh and safe:)")
    print("-----------------------------------------------------")
    time.sleep(1)

    while(1):
        fingerID, ifExist = user_loggin()

        if not os.path.exists("/home/pi/Documents/lineBot/regist_reply_token") : # 沒有人按註冊
            user_id = get_userID(fingerID)
            if (user_id != -1):

                # 閃燈前關閉綠燈
                GPIO.output(LED_GREEN_OFFSET, GPIO.LOW)

                # 閃燈開始
                p = Process(target=led_blink , args=(LED_RED_OFFSET,))

                # 執行該子執行緒
                p.start()

                time.sleep(2)
                
                food_type = take_pic()
                food_type_synoposys = get_em9d_result()

                # 閃燈結束
                p.terminate()

                # 關閉紅燈
                GPIO.output(LED_RED_OFFSET, GPIO.LOW) 

                # 開關 看insert 或 取出
                flag_insert_delete = GPIO.input(BUTTON_OFFSET)

                # 從image位置抓file辨識文字 (google_api.py)
                # 放入食物
                if(flag_insert_delete):
                    exp_time = exp_time_api()
                    if( insert_food(food_type, user_id, exp_time) ):
                        # 取出成功
                        # 亮燈
                        print("insert succ")
                        p = Process(target=led_light_up , args=(LED_GREEN_OFFSET,))
                    else:
                        # 取出失敗
                        # not your food
                        # 亮燈
                        print("insert fail")
                        p = Process(target=led_light_up , args=(LED_RED_OFFSET,))
                # 取出食物 
                else:
                    if( take_food(food_type, user_id) ):
                        # 取出成功
                        # 亮燈
                        print("take succ")
                        p = Process(target=led_light_up , args=(LED_GREEN_OFFSET,))
                    else:
                        # 取出失敗
                        # not your food
                        # 亮燈
                        print("take fail")
                        p = Process(target=led_light_up , args=(LED_RED_OFFSET,))
        else: # 有人按註冊
            # res = check_fingerID_exist(fingerID)
            if ifExist == False:
                fingerID = regist_finger()

                if fingerID == -1:
                    content = TextSendMessage(text="指紋註冊失敗\n請重新註冊")   
                else:
                    user_id = None
                    display_name = None
                    with open("/home/pi/Documents/lineBot/regist_user_id") as f:
                        user_id = f.readline()
                    with open("/home/pi/Documents/lineBot/regist_display_name") as f:
                        display_name = f.readline()

                    r = register_fingerID(fingerID, user_id, display_name)
                    print(f'line 181 res for register_fingerID: {r}')
                    if r:
                        content = TextSendMessage(text="指紋註冊成功！您的帳戶建立完成")
                    else:
                        content = TextSendMessage(text="指紋註冊失敗\n此指紋已被註冊過")
            else:
                content = TextSendMessage(text="指紋註冊失敗\n此指紋已被註冊過")
            
            with open("/home/pi/Documents/lineBot/regist_reply_token") as f:
                regist_reply_token = f.readline() 
                line_bot_api.reply_message(
                    regist_reply_token,
                    content)
        
            os.remove("/home/pi/Documents/lineBot/regist_reply_token")
            os.remove("/home/pi/Documents/lineBot/regist_user_id")
            os.remove("/home/pi/Documents/lineBot/regist_display_name")

from linebot import (
    LineBotApi, WebhookHandler
)
from linebot.exceptions import (
    InvalidSignatureError
)
from linebot.models import (
    MessageEvent, TextMessage, TextSendMessage, TemplateSendMessage, ConfirmTemplate, PostbackAction, MessageAction, MessageTemplateAction, ButtonsTemplate
)
# 宣告一個變數負責掌控server
app = Flask(__name__)

path=sys.argv[0]
print(path)

fingerid_status = ""
# 一次只有一個人能註冊（防呆，不影響demo)
anyone_register = False

#要改channel_secret和channel_access_token
channel_secret = 'aa6996455342ff785784d16fb80b51b1'
channel_access_token = 'zT3Ntk/diLJogBsqtbw6fJvZswRtsOfTjD5Fn8QxbCZCbwt/5n5WPbp7OaxREi1yn3UzW09CQFn40HO03UHwJvh/26aaRu0RN3lmNQ1EbY1dhkEOX6DLgHPCCmwFu8Aem4MeeKIDSqPiolVfiR+cqAdB04t89/1O/w1cDnyilFU='
if channel_secret is None:
    print('Specify LINE_CHANNEL_SECRET as environment variable.')
    sys.exit(1)
if channel_access_token is None:
    print('Specify LINE_CHANNEL_ACCESS_TOKEN as environment variable.')
    sys.exit(1)

line_bot_api = LineBotApi(channel_access_token)
handler = WebhookHandler(channel_secret)

def test():
    confirm_template_message = TemplateSendMessage(
        alt_text='Confirm template',
        template=ConfirmTemplate(
            text='Are you sure?',
            actions=[
                PostbackAction(
                    label='postback',
                    display_text='postback text',
                    data='action=buy&itemid=1'
                ),
                MessageAction(
                    label='message',
                    text='message text'
                )
            ]
        )
    )
    return confirm_template_message
# Define the callback function for handling incoming messages
def callback(ch, method, properties, body):
    print('Received message:', body)


@app.route("/callback", methods=['POST'])
def callback():
    # get X-Line-Signature header value
    signature = request.headers['X-Line-Signature']

    # get request body as text
    body = request.get_data(as_text=True)
    app.logger.info("Request body: " + body)

    # handle webhook body
    try:
        handler.handle(body, signature)
    except InvalidSignatureError:
        abort(400)

    return 'OK'

@app.route("/")
def control_led():
    global fingerid_status
    # 
    global anyone_register
    return fingerid_status

@handler.add(MessageEvent, message=TextMessage)
def message_text(event):
    global fingerid_status
    
    con = sqlite3.connect("fridge1.db")
    cur = con.cursor()
    sql_query = """SELECT name FROM sqlite_master WHERE type='table';"""
    res = cur.execute("SELECT * FROM USER")
    # res = cur.execute(sql_query)
    print(res)
    print(res.fetchone())
    print(res.fetchone() is None)
    user_id = event.source.user_id
    print("User ID:", user_id)
    print (f'event.message.text:\t{event.message.text}')


    profile = line_bot_api.get_profile(user_id)

    print(f'display_name: {profile.display_name}')
    print(f'user_id: {profile.user_id}')
    print(f'picture_url: {profile.picture_url}')
    print(f'status_message: {profile.status_message}')


    if event.message.text.lower() == "註冊":
        if os.path.exists("/home/pi/Documents/lineBot/regist_reply_token"):
           content = TextSendMessage(text="有人在註冊請稍後再試")
        else:  
           content = enroll(profile.user_id)
        
        line_bot_api.reply_message(
            event.reply_token,
            content)
        # line_bot_api.reply_message(
        #     event.reply_token,
        #     TextSendMessage(text=content))
        return 0
    if event.message.text.lower() == "xxxx":
        content = TemplateSendMessage(
            alt_text='Confirm template2',
            template=ConfirmTemplate(
                text='請將手指至於指紋辨識器上\n並按下確定鍵等待掃描',
                actions=[
                    PostbackAction(
                        label='cancel finger regi',
                        display_text='取消',
                        data='action=buy&itemid=1'
                    ),
                    MessageAction(
                        label='register finger ',
                        text='指紋確認'
                    )
                ]
            )
        )

        
        line_bot_api.reply_message(
            event.reply_token,
            content)
        
        return 0
    if event.message.text.lower() == "確定註冊":
        # # Connect to RabbitMQ server
        # connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
        # channel = connection.channel()
        # # Declare the message queue
        # channel.queue_declare(queue='trigger_queue')
        # # Wait for a specific time period (e.g., 5 seconds)
        # time.sleep(5)
        # # Get a single message from the queue
        # method_frame, properties, body = channel.basic_get(queue='trigger_queue', auto_ack=True)
        # Check if a message is received
        # if method_frame is not None:
        #     print('Received message:', body)
        #     content = TextSendMessage(text="指紋註冊成功")
            
        # else:
        #     print('No messages in the queue.')
        #     content = TextSendMessage(text="指紋註冊失敗")
        # # Close the connection
        # connection.close()
        
        # todo: claire api
        with open("/home/pi/Documents/lineBot/regist_reply_token", 'w') as f:
            f.write(event.reply_token)
        with open("/home/pi/Documents/lineBot/regist_user_id", 'w') as f:
            f.write(profile.user_id)
        with open("/home/pi/Documents/lineBot/regist_display_name", 'w') as f:
            f.write(profile.display_name)
        
        return 0
    if event.message.text == "查看我的食物":
        
        content = my_food(profile.user_id)
        line_bot_api.reply_message(
            event.reply_token,
            content)
        return 0
    if event.message.text == "最快過期食物":
        content = expiration(profile.user_id)
        line_bot_api.reply_message(
            event.reply_token,
            TextSendMessage(text=content))
        return 0
 
# --------------------------------放入食物----------------------------------------   
    if event.message.text == "放入食物":
        res = check_login(profile.user_id)
        if res is None:
            content = "您還沒有註冊過 請先註冊"
            line_bot_api.reply_message(
                event.reply_token,
                TextSendMessage(text=content))
        else: 
            content = TemplateSendMessage(
                alt_text='Confirm template3',
                template=ConfirmTemplate(
                    text='請將手指至於指紋辨識器上\n並按下確定鍵等待掃描',
                    actions=[
                        PostbackAction(
                            label='取消',
                            display_text='取消',
                            data='action=buy&itemid=1'
                        ),
                        MessageAction(
                            label='確認 ',
                            text='指紋確認＿放食物'
                        )
                    ]
                )
            )

            line_bot_api.reply_message(
                event.reply_token,
                content)
        return 0
    if event.message.text == "指紋確認＿放食物":

        fingerID, ifExist = user_loggin()
        
        # res = check_fingerID_exist(fingerID)

        if fingerID:
             
            content = TemplateSendMessage(
                alt_text='Confirm template4',
                template=ConfirmTemplate(
                    text='請將食品至於鏡頭範圍後\n並按下確定鍵等待拍照',
                    actions=[
                        PostbackAction(
                            label='取消',
                            display_text='取消',
                            data='action=buy&itemid=1'
                        ),
                        MessageAction(
                            label='確認',
                            text='拍照確認＿放食物'
                        )
                    ]
                )
            )
            line_bot_api.reply_message(
                event.reply_token,
                content)
        return 0
    
    if event.message.text == "拍照確認＿放食物":
        # read image
        # 送圖到localhost或pi或心思辨識圖片
        food_type = fake_api()
        food_type = take_pic()
        user_id = get_userIDby_lineid(profile.user_id)
        exp_time = exp_time_api()
        res = insert_food(food_type, user_id, exp_time)
        if res:
            content = "成功 放入食物 XD"
        else: 
            content = "放食物失敗 :("

        line_bot_api.reply_message(
            event.reply_token,
            TextSendMessage(text=content))
        return 0

# --------------------------------拿出食物----------------------------------------
    if event.message.text == "拿取食物":
        res = check_login(profile.user_id)
        
        if res is None:
            content = "您還沒有註冊過 請先註冊"
            line_bot_api.reply_message(
                event.reply_token,
                TextSendMessage(text=content))
        else: 
            userId = get_userIDby_lineid(profile.user_id)
            rows = food_detail(userId)
            if(len(rows) > 0):

                content = TemplateSendMessage(
                    alt_text='Confirm template3',
                    template=ConfirmTemplate(
                        text='請將手指至於指紋辨識器上\n並按下確定鍵等待掃描',
                        actions=[
                            PostbackAction(
                                label='取消',
                                display_text='取消',
                                data='action=buy&itemid=1'
                            ),
                            MessageAction(
                                label='確認',
                                text='指紋確認＿拿食物'
                            )
                        ]
                    )
                )
            else: 
                content = TextSendMessage(text=f'您還沒有任何食物放在冰箱')
                
            line_bot_api.reply_message(
                event.reply_token,
                content)
        return 0
    
    if event.message.text == "指紋確認＿拿食物":        
        fingerID, ifExist = user_loggin()
        if fingerID:    
            content = TemplateSendMessage(
                alt_text='Confirm template4',
                template=ConfirmTemplate(
                    text='請將食品至於鏡頭範圍後\n並按下確定鍵等待拍照',
                    actions=[
                        PostbackAction(
                            label='取消',
                            display_text='取消',
                            data='action=buy&itemid=1'
                        ),
                        MessageAction(
                            label='確認',
                            text='拍照確認＿拿食物'
                        )
                    ]
                )
            )
            line_bot_api.reply_message(
                event.reply_token,
                content)
        return 0
    if event.message.text == "拍照確認＿拿食物":
        # read image
        # 送圖到localhost或pi或心思辨識圖片
        food_type = fake_api()
        food_type = take_pic()
        user_id = get_userIDby_lineid(profile.user_id)
        # exp_time = exp_time()
        res = take_food(food_type, user_id)
        if res:
            content = "成功 拿出食物 XD"
        else: 
            content = "拿食物失敗 :("

        line_bot_api.reply_message(
            event.reply_token,
            TextSendMessage(text=content))
        return 0
# --------------------------------多餘----------------------------------------
    if event.message.text == "待開發功能":
        buttons_template = TemplateSendMessage(
            alt_text='template',
            template=ButtonsTemplate(
                title='服務類型',
                text='請選擇',
                thumbnail_image_url='https://i.imgur.com/sbOTJt4.png',
                actions=[
                    MessageTemplateAction(
                        label='服務一',
                        text='服務一'
                    ),
                    MessageTemplateAction(
                        label='服務二',
                        text='服務二'
                    ),
                    MessageTemplateAction(
                        label='服務三',
                        text='服務三'
                    )
                ]
            )
        )
        line_bot_api.reply_message(event.reply_token, buttons_template)
        return 0
    
    if event.message.text == "a":
        content = test()
        line_bot_api.reply_message(
            event.reply_token,
            content)
        return 0
    else:
        line_bot_api.reply_message(
            event.reply_token,
            TextSendMessage(text="請用選單操作")
        )
    

if __name__ == "__main__":
    arg_parser = ArgumentParser(
        usage='Usage: python ' + __file__ + ' [--port <port>] [--help]'
    )
    arg_parser.add_argument('-p', '--port', default=8000, help='port')
    arg_parser.add_argument('-d', '--debug', default=False, help='debug')
    options = arg_parser.parse_args()

    regist_reply_token = None
    regist_profile = None

    init_led()

    finger_detect_thread = threading.Thread(target = finger_detect)
    finger_detect_thread.start()

    app.run(debug=options.debug, port=options.port)
