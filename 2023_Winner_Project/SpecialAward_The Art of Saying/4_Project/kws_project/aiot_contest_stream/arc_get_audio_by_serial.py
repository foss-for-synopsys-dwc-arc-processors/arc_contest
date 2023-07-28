import argparse
import serial
import requests
import json
import numpy as np
from scipy.io.wavfile import write
from time import sleep

parser = argparse.ArgumentParser()
parser.add_argument("--com", nargs='?', default="/dev/ttyUSB1")
parser.add_argument("--audio", nargs='?', default="pdm.wav")
args = parser.parse_args()

LOCALHOST = 'http://127.0.0.1:8080/recognition'
def send_audio(audio, url: str = LOCALHOST) -> requests.Response:
	headers = {
		"Authorization": "Bearer %_put_your_auth_token_here_%",
	}
	with open(audio, 'rb') as file:
		files = {'audio': file}
		p = requests.Request('POST', url=url, headers=headers, files=files).prepare()
		s = requests.Session()
		r = s.send(p, verify=False)

	return r

class arc_serial:
	def __init__(self, tty_name):
		self.ser = serial.Serial()
		self.ser.port = tty_name
		# If it breaks try the below
		self.serConf() # Uncomment lines here till it works

		self.ser.open()
		self.ser.flushInput()
		self.ser.flushOutput()

		self.addr = None

	def serConf(self):
		self.ser.baudrate = 115200
		self.ser.bytesize = serial.EIGHTBITS
		self.ser.parity = serial.PARITY_NONE
		self.ser.stopbits = serial.STOPBITS_ONE
		self.ser.timeout = 0 # Non-Block reading
		self.ser.xonxoff = False # Disable Software Flow Control
		self.ser.rtscts = False # Disable (RTS/CTS) flow Control
		self.ser.dsrdtr = False # Disable (DSR/DTR) flow Control
		self.ser.writeTimeout = 2

	def close(self):
		self.ser.close()

	def cmd(self, cmd_str, encode=''):
		_str = cmd_str
		self.ser.write(_str)

	def monitor_audio_data(self, audio_filename):
		st_tag = "-start-"
		end_tag = "-end-"

		while True:
			if self.ser.in_waiting:
				line = self.ser.readline().decode()
				if len(line) > 3:
					string = line[:-2]
					print(string)
					if string == st_tag:
						channel = np.array([])
						while True:
							if self.ser.in_waiting:
								line = self.ser.readline().decode()
								if len(line) > 3:
									string = line[:-2]
									print(string)
									if string.isdigit():
										val = int(string)
										if val < 32767:
											channel = np.append(channel, [val], axis=0)
										#print(val)
									elif string.startswith("-") and string[1:].isdigit():
										val = int(string[1:])*-1
										if val > -32767:
											channel = np.append(channel, [val], axis=0)
										#print(val)
									elif string == end_tag:
										print(f"write {audio_filename}")
										samplerate = 16000
										write(audio_filename, samplerate, channel.astype(np.int16))

										res = send_audio(audio_filename, 'https://35.221.137.170/recognition')
										try:
											print(res.text)
											j = json.loads(res.text)
											if j["EID"] == "unknown":
												self.cmd(b"f")
												print("return FAIL\n\n\n\n\n")
											else:
												self.cmd(b"p")
												print("return PASS\n\n\n\n\n\n")
										except:
											pass

										channel = np.delete(channel, range(0,len(channel)))
										break

if __name__ == '__main__':
	s = arc_serial(args.com)

	s.monitor_audio_data(args.audio)

	s.close()
	# res = send_audio(args.audio, 'https://35.221.137.170/recognition')
	# print(res.text)
