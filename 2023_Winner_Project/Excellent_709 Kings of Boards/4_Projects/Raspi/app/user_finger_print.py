# This is a interface for user log in with fingerprint

# This interface uses module from
# /home/pi/Documents/709/fingure_print_module



import binascii
import serial
import time

from fingure_print_module.utils import sendcmd, init, searchfig, disfig, waitfig, savefig, deletfig






# the default uart for raspi gpio 14, 15
# ser = serial.Serial("/dev/ttyS0", 57600)


# the file that record the current finger id
FINGER_ID_PATH = "/home/pi/Documents/709/user_finger_print_id.txt"



# 這是一個給user登入時
# 呼叫的指紋辨識module

## user 押上指紋
## module 會判斷是否為新用戶
## if true-> register now
## if false -> have registered before
## return (finger id, bool(yes. no))


def regist_finger():
    f = open(FINGER_ID_PATH, "r")
    
    # _id will be an interger between 1-10
    _id = (int(f.read())) %11 + 1
    print(f"regist finger_id: {_id}")
    
    savefig(_id)
    
    f = open(FINGER_ID_PATH, "w")
    # the _id may overwrites the current registered id 
    f.write(str(_id))

    return _id

def user_loggin():

    # to initialize the finger print module
    init()
    print('Finger print module initialization successful')
    time.sleep(0.1)
    
    serach_result = disfig()
    
    print(f"serach_result: {serach_result}")

    if (serach_result == "No matching fingerprint found"):
        return (-1, False)
    else:
        return (serach_result, True)
