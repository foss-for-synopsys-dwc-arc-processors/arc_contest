# import cv2

# cam = cv2.VideoCapture(0)

# while True:
# 	ret, image = cam.read()
# 	cv2.imshow('Imagetest',image)
# 	k = cv2.waitKey(1)
# 	if k != -1:
# 		break
# cv2.imwrite('/home/pi/testimage.jpg', image)
# cam.release()
# cv2.destroyAllWindows()

import os
import sys
import re


os.system("fswebcam -d /dev/video0 --no-banner -r 1080x720 image.jpg")