# %%
from matplotlib import pyplot as plt
import numpy as np
import os
import pandas as pd
import re
import serial
import scipy.optimize
# %%
ser = serial.Serial('COM7', 115200)

tflite_predict = []
x = []
y = []
z = []
fig, ax = plt.subplots(2, 2)
read_finish = False

# %%
def saveAsNPZ(x, y, z, tflite_predict, fname='archive.npz'):
	x = np.array(x).reshape(-1,1)
	y = np.array(y).reshape(-1,1)
	z = np.array(z).reshape(-1,1)
	tflite_predict = np.array(tflite_predict)
	np.savez(f'nparray/{fname}', x=x, y=y, z=z, ref=tflite_predict)


# ref: https://stackoverflow.com/questions/16716302/how-do-i-fit-a-sine-curve-to-my-data-with-pylab-and-numpy
def fit_sin(tt, yy):
	'''Fit sin to the input time sequence, and return fitting parameters "amp", "omega", "phase", "offset", "freq", "period" and "fitfunc"'''
	tt = np.array(tt)
	yy = np.array(yy)
	ff = np.fft.fftfreq(len(tt), (tt[1]-tt[0]))   # assume uniform spacing
	Fyy = abs(np.fft.fft(yy))
	guess_freq = abs(ff[np.argmax(Fyy[1:])+1])   # excluding the zero frequency "peak", which is related to offset
	guess_amp = np.std(yy) * 2.**0.5
	guess_offset = np.mean(yy)
	guess = np.array([guess_amp, 2.*np.pi*guess_freq, 0., guess_offset])

	def sinfunc(t, A, w, p, c):  return A * np.sin(w*t + p) + c
	popt, pcov = scipy.optimize.curve_fit(sinfunc, tt, yy, p0=guess)
	A, w, p, c = popt
	f = w/(2.*np.pi)
	fitfunc = lambda t: A * np.sin(w*t + p) + c
	return {"amp": A, "omega": w, "phase": p, "offset": c, "freq": f, "period": 1./f, "fitfunc": fitfunc, "maxcov": np.max(pcov), "rawres": (guess,popt,pcov)}

def count_repeated_times(arr):
	repeated_times_dict = {"0":[], "1":[]}
	max_time = 0
	curr_time = 1
	pre_ele = None
	for ele in arr:
		if ele == pre_ele:
			curr_time += 1
			max_time = max(curr_time, max_time)
		else:      
			if pre_ele != None:
				repeated_times_dict[f'{int(pre_ele)}'].append(max_time)
				max_time = 1
			pre_ele = ele
			curr_time = 1
	return repeated_times_dict

def duty_factor(contact_time, aerial_time):  return contact_time/(contact_time + aerial_time)
# %%
while not read_finish:
	while ser.in_waiting:
		raw_line = ser.readline()
		line = str(raw_line.decode()).replace("\r\n", "")
		print(line)
		if line.startswith('idx'):
			m = re.match(r'idx\d+ (-?\d\.\d+) (-?\d\.\d+) (-?\d\.\d+)', line)
			# print(m.groups())
			x.append(float(m.groups()[0]))
			y.append(float(m.groups()[1]))
			z.append(float(m.groups()[2]))

		if line.startswith('Round'):
			m = re.match(r'Round.+?(\d+): (\d)', line)
			# print(m.groups())
			tflite_predict.append(int(m.groups()[1]))
			if m.groups()[0] == '1003':
				print(len(tflite_predict), len(x), len(y), len(z), sep=",")
				saveAsNPZ(x, y, z, tflite_predict, '07-16.npz')
				ax[0][0].set_title('predict')
				
				xx = np.where(np.array(tflite_predict) == 1)[0]
				for _x in xx:
					ax[0][0].axvline(x= _x, color = 'b', lw=0.1)

				ax[0][1].plot(x)
				ax[0][1].set_title('x')

				ax[1][0].plot(y)
				ax[1][0].set_title('y')

				ax[1][1].plot(z)
				ax[1][1].set_title('z')

				plt.tight_layout()
				plt.show()

				plt.plot(tflite_predict, label='origin prediction')
				x_range = np.linspace(0, 1004, 1004)
				res = fit_sin(x_range, tflite_predict)
				plt.plot(x_range, res["fitfunc"](x_range), label='sine wave to fit')
				plt.axhline(y=0.5, color='r', linestyle='dotted', label='threshold y=0.5')
				recons_class_res = np.where(res["fitfunc"](x_range)>0.5, 1., 0)
				plt.plot(recons_class_res, ls='dashed', lw=4, label='reconstruct prediction')
				plt.legend(loc='upper right')
    
				repeated_times_dict = count_repeated_times(tflite_predict)
				print(repeated_times_dict)
    
				contact_count = sorted(repeated_times_dict["1"], reverse=True)[0]
				aerial_count = sorted(repeated_times_dict["0"], reverse=True)[0]
				plt.text(0., 0., f'Reconstruct Duty Factor: {duty_factor(contact_count, aerial_count):.2f}', size='x-large')
				plt.show()

				read_finish = True
				break

# %%
ser.close()
print('Good-Byeeeeeee')

# # %%
# myArch = np.load('nparray/07-16.npz')
# x = myArch['x']
# y = myArch['y']
# z = myArch['z']
# ref = myArch['ref']
# # %%
# fig, ax = plt.subplots(2, 2)

# ax[0][0].plot(ref)
# ax[0][0].set_title('predict')

# ax[0][1].plot(x)
# ax[0][1].set_title('x')

# ax[1][0].plot(y)
# ax[1][0].set_title('y')

# ax[1][1].plot(z)
# ax[1][1].set_title('z')

# plt.tight_layout()
# plt.show()
# # %%
