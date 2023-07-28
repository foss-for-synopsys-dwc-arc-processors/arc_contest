

import binascii
import serial
import time

from fingure_print_module.utils import sendcmd, init, searchfig, disfig, waitfig, savefig, deletfig

for num in range(11):
    deletfig(num)