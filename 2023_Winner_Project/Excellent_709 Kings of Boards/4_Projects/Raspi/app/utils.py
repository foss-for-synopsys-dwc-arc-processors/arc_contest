# from asyncio.windows_events import NULL
import sqlite3
from datetime import datetime
from google_api import exp_time_api
from linebot.models import (
    MessageEvent, TextMessage, TextSendMessage, TemplateSendMessage, ConfirmTemplate, PostbackAction, MessageAction, MessageTemplateAction, ButtonsTemplate
)
# check if the fingerprint exist in the database
def check_fingerID_exist(fingerID):
    con = sqlite3.connect("fridge1.db")
    cur = con.cursor()
    res = cur.execute(f"SELECT * FROM USER where finger_id = '{fingerID}'")
    print(f'line 12:\t{res}')
    
    if(res.fetchone()):
        return True
    else:
        return False
def register_fingerID(fingerID, line_id, user_name):
    
    con = sqlite3.connect("fridge1.db")
    cur = con.cursor()
    cmd = f"INSERT INTO USER(finger_id, line_id, user_name) VALUES ({fingerID}, '{line_id}', '{user_name}')"
    try:
        print(cmd)
        res = cur.execute(cmd)
        con.commit()
        if cur.rowcount > 0:
            print("Insertion successful")
            return True
        else:
            print("Insertion failed")
            return False
    except sqlite3.IntegrityError as e:
        print("Insertion error:", e)
def check_login(user_id):

    con = sqlite3.connect("fridge1.db")
    cur = con.cursor()
    res = cur.execute(f"SELECT * FROM USER where line_id = '{user_id}'")
    # print(f'line 12:\t{res}')
    return res.fetchone()
    if (res.fetchone()):
        user_id, finger_id, line_id, user_name= res.fetchone()
        print(f'hello. {user_id}, {finger_id}, {line_id}, {user_name}')
        return user_id, finger_id, line_id, user_name
    else:
        return None
def enroll(user_id):
    
    result = check_login(user_id)
    if(result is None):
        
        # print(f'I have your user id. {user_id}')
        # con = sqlite3.connect("fridge1.db")
        # cur = con.cursor()
        # res = cur.execute(f"SELECT * FROM USER where line_id = {user_id}")
        # print(res)
    
        content = TemplateSendMessage(
            alt_text='Confirm template',
            template=ConfirmTemplate(
                text='請將手指至於指紋辨識器上\n並按下確定鍵等待掃描',
                actions=[
                    PostbackAction(
                        label='取消註冊',
                        display_text='取消註冊',
                        data='action=buy&itemid=1'
                    ),
                    MessageAction(
                        label='確定註冊',
                        text='確定註冊'
                    )
                ]
            )
        )
    else:
        user_id, finger_id, line_id, user_name = result
        
        content = f'您已註冊過.\n您的帳號資訊如下：\n  使用者名稱：{user_name}\n  使用者id: {user_id}\n  指紋id: {finger_id}'
        print(content)
        content = TextSendMessage(text=content)
    return content

def food_detail(user_id):
    con = sqlite3.connect("fridge1.db")
    cur = con.cursor()
    
    res = cur.execute(f"SELECT * FROM USER_FOOD where user_id = '{user_id}'")
    # res = cur.execute(f"SELECT * FROM USER_FOOD where user_id = '1'")
    # print(f'line 62:\t{res}')
    # Fetch all rows from the result
    rows = cur.fetchall()
    # print(f'line 65 rows:\t{rows}')
    # Iterate over the rows
    if rows:
        for row in rows:
            # row might be type: tuple
            # Access individual columns using indexing
            food_id = row[0]
            user_id = row[1]
            food_type = row[2]
            exp_time = row[3]
            print(f'{type(food_id)}, {type(user_id)}, {type(food_type)}, {type(exp_time)}')

    return rows

def my_food(user_id):
    # check_login 的user_id是line_id
    result = check_login(user_id)
    if(result):
        user_id, finger_id, line_id, user_name = result
        rows = food_detail(user_id)
        # print(f"line 85: {type(rows)}")
        # print(f"line 85: {rows}")
        
        content = f"您有 {len(rows)} 件食物存放在冰箱\n"
        for i, row in enumerate(rows):
            content += f"{i+1}. {row[2]}, 截止日期: {row[3]}\n"
        print(content)
        content = TextSendMessage(text=content)
    else: 
        content = "您尚未註冊過"
        content = TextSendMessage(text=content)
    return content

# 收到line id 回傳要過期的食物
def expiration(user_id):

    result = check_login(user_id)
    if(result):
        user_id, finger_id, line_id, user_name = result
        rows = food_detail(user_id)
        content = "expiration"

 
        # Get the current date and time
        current_time = datetime.now()

        # Format the current date and time
        formatted_time_today = current_time.strftime("%Y-%m-%d")
        if(len(rows) > 0):
            content = f"{'-'*20}\n"
        else:
            content = "您沒有要過期的食物"
        date1 = datetime.strptime(formatted_time_today, "%Y-%m-%d")
        for row in rows:
            if(row[3] is None):
                continue
            else:
                date2 = datetime.strptime(row[3], "%Y-%m-%d")
            # Compare the dates
            if date1 < date2:
                delta = date2 - date1
                num_days = delta.days
                if num_days<5:
                    content += f"食物: {row[2]} 在 {num_days} 天內過期\n{'-'*20}\n"
                    print(f"food may exp in {num_days}.")
            elif date1 > date2:
                delta = date1 - date2
                num_days = delta.days
                content += f"食物: {row[2]} 已過期 {num_days} 天\n{'-'*20}\n"
                print(f"food already exp {num_days}")
            else:
                print("food will exp today.")
                content += f"食物: {row[2]} 今天過期\n{'-'*20}\n"
        
        print(content)

    else: 
        content = "您尚未註冊過"
    return content

# 用手指id找 user id
def get_userID(finger_id):
    con = sqlite3.connect("fridge1.db")
    cur = con.cursor()
    print(f'finger_id: {finger_id}')
    # todo get user_id
    cmd = f"SELECT * FROM USER where finger_id = '{finger_id}'"
    try:
        # Execute the insertion command
        cur.execute(cmd)
        
        result = cur.fetchone()

        if result is not None:
            # Query returned at least one row
            print("Query successful")
            # Process the retrieved data as needed
            print(result)
            user_id = result[0]
            line_id = result[2]
        else:
            # Query did not return any rows
            user_id = -1
            print("Query returned no results")

        # Close the connection
        con.close()
    except sqlite3.IntegrityError as e:
        print("Selection error:", e)
    return user_id
def get_userIDby_lineid(line_id):
    con = sqlite3.connect("fridge1.db")
    cur = con.cursor()
    # todo get user_id
    cmd = f"SELECT * FROM USER where line_id = '{line_id}'"
    try:
        # Execute the insertion command
        cur.execute(cmd)
        
        result = cur.fetchone()

        if result is not None:
            # Query returned at least one row
            print("Query successful")
            # Process the retrieved data as needed
            print(result)
            user_id = result[0]
            line_id = result[2]
        else:
            # Query did not return any rows
            print("Query returned no results")
            return -1
        # Close the connection
        con.close()
    except sqlite3.IntegrityError as e:
        print("Selection error:", e)
    return user_id
# 插入食物 food_type, user_id, exp_time
def insert_food(food_type, user_id, exp_time):
    # exp_time :None or string
    con = sqlite3.connect("fridge1.db")
    cur = con.cursor()
    # todo get user_id
    if exp_time is None:
        cmd = f"INSERT INTO USER_FOOD (user_id,food_type) VALUES ('{user_id}', '{food_type}')"
    else:
        cmd = f"INSERT INTO USER_FOOD (user_id,food_type, exp_time) VALUES ('{user_id}', '{food_type}', '{exp_time}')"
    
    print(cmd)
    try:
        # Execute the insertion command
        cur.execute(cmd)
        con.commit()
        # Check if the insertion was successful
        if cur.rowcount > 0:
            print("Insertion of food successful.")
            return True

        else:
            print("Insertion of food failed.")
            return False

        # Commit the changes to the database
        con.commit()
        # Close the connection
        con.close()
        return True

    except sqlite3.IntegrityError as e:
        print("Insertion error:", e)

def take_food(food_type, user_id):
    con = sqlite3.connect("fridge1.db")
    cur = con.cursor()
    # todo get user_id
    cmd = f"DELETE FROM USER_FOOD WHERE user_id = '{user_id}' AND food_type = '{food_type}' "
    
    try:
        # Execute the insertion command
        cur.execute(cmd)

        # Check if the insertion was successful
        if cur.rowcount > 0:
            print("Deletion of food successful.")
            return True

        else:
            print("Deletion of food failed.")
            return False

        # Commit the changes to the database
        con.commit()
        # Close the connection
        con.close()
        return True

    except sqlite3.IntegrityError as e:
        print("Insertion error:", e)

    
    

def fake_api():
    return "guava"