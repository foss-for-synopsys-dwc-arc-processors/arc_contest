import socket
import sys
from PyQt5.QtWidgets import QDialog, QVBoxLayout, QHBoxLayout, QSizePolicy, QApplication, QSizePolicy, QLabel, QWidget
from PyQt5.QtCore import QTimer, Qt
from matplotlib.figure import Figure
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.ticker import MultipleLocator
import random
import time
import numpy as np
import matplotlib.patches as patches
from matplotlib.animation import FuncAnimation
from PyQt5.QtGui import QFont
import pandas as pd

class MyMplCanvas(FigureCanvas):
    def __init__(self, parent=None, width=5, height=4, dpi=100):
        self.fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = self.fig.add_subplot(111)

        FigureCanvas.__init__(self, self.fig)
        self.setParent(parent)

        FigureCanvas.setSizePolicy(self,
                QSizePolicy.Expanding,
                QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(self)

        self.fig.patch.set_facecolor('white')
        self.axes.set_facecolor('white')
        self.axes.spines['bottom'].set_color('black')
        self.axes.spines['top'].set_color('black') 
        self.axes.spines['right'].set_color('black')
        self.axes.spines['left'].set_color('black')

        self.axes.xaxis.label.set_color('black')
        self.axes.yaxis.label.set_color('black')
        self.axes.tick_params(axis='x', colors='black')
        self.axes.tick_params(axis='y', colors='black')

        self.axes.grid(color='gray', linestyle='--', linewidth=0.5)

        self.data = []
        self.times = []

    def plot(self, y):
        self.data.append(y)
        self.times.append(0)

        while self.times and self.times[0] < -30:
            self.times.pop(0)
            self.data.pop(0)

        self.axes.clear()
        self.axes.set_facecolor('white')
        self.axes.grid(color='gray', linestyle='--', linewidth=0.5)

        self.axes.step(self.times, self.data, 'g-', where='post')
        self.axes.fill_between(self.times, self.data, 0, color='green', alpha=0.3, step='post')
        
        self.axes.set_xlim([-30, 0])
        self.axes.set_ylim([0, 5])
        self.axes.set_xlabel('Time (s)', color='black', fontsize=16)
        self.axes.set_ylabel('Value (bar)', color='black', fontsize=16)
        self.axes.set_title('Historical Data', color='black', fontsize=24)

        self.axes.yaxis.set_major_locator(MultipleLocator(0.5))

        self.draw()
        
        # Shift the old data to the left (會需要根據實際情況調整更新秒數) (有調位置)
        self.times = [t - 1.5 for t in self.times]

# 動畫的指針變化
class GaugeCanvas(FigureCanvas):
    def __init__(self, parent=None, width=5, height=4, dpi=100):
        self.fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = self.fig.add_subplot(111, polar=True)
        self.axes.set_yticklabels([])
        self.axes.set_title("Gauge Degree", color='black', fontsize=24)  # 給儀表指針的圖命名
        
        # 設定刻度的間隔為15度，並且轉換為弧度
        self.axes.set_xticks(np.deg2rad(np.arange(0, 360, 10)))
        for theta in np.deg2rad(np.arange(5, 360, 10)):
            self.axes.scatter(theta, 0.5, color='gray')
        
        # 設定刻度標籤的範圍為0~345度，間隔為15度
        labels = [(label + 100) % 360 for label in range(0, 360, 10)]
        self.axes.set_xticklabels(labels[::-1], color = 'black', fontsize=16)
        self.axes.set_xticklabels([], minor=True)

        self.axes.set_rticks([])
        self.axes.spines['polar'].set_visible(False)
        self.axes.grid(color='gray', linestyle='--', linewidth=0.5)
        self.axes.set_facecolor('white')
        self.fig.patch.set_facecolor('white')

        FigureCanvas.__init__(self, self.fig)
        self.setParent(parent)

        FigureCanvas.setSizePolicy(self,
                QSizePolicy.Expanding,
                QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(self)

        self.arrow, = self.axes.plot([0, 0], [0, 0.5], color='green', linewidth=2)  # 將指針的線寬設為2

        self.target_theta = 0
        self.current_theta = 0
        self.anim = FuncAnimation(self.fig, self._update_frame, frames=np.arange(0, 4), interval=50) # interval: 50 seconds between each frame

    def _adjust_angle(self, y):
        # 將角度減去90，再轉換為弧度，並且取負數以確保在順時針方向旋轉
        return -np.radians(y - 90)

    def plot(self, y):
        self.target_theta = self._adjust_angle(y)
        
    def _update_frame(self, i):
        if self.current_theta != self.target_theta:
            # 將角度差分成N等分，依據當前的動畫畫面，選擇對應的角度
            self.current_theta += (self.target_theta - self.current_theta) / 4 # N ，如果更新時間不算長是能夠讓它變更大的 0.5s建議不要用動畫，這個值需要等於frame 的max value
            
            self.arrow.set_data([0, self.current_theta], [0, 0.5])  # 更新指針的位置
            self.draw()


# 在主窗口中加入新的儀表圖
class ApplicationWindow(QDialog):
    def __init__(self):
        super().__init__()
        self.initUI()
        self.val = 0
        # self.setStyleSheet("background-color: black")  # 設定 QDialog 的背景顏色為黑色，開啟這個會讓整個GUI背景全黑相連

    def initUI(self):
        self.setWindowTitle("Gauge Historical Data")
        self.setGeometry(240, 240, 1800, 900)

        self.canvas = MyMplCanvas(self, width=5, height=4, dpi=100)  # 原來的階梯圖
        self.gauge = GaugeCanvas(self, width=5, height=4, dpi=100)  # 新的儀表圖

        # 新增的 label 設定
        self.label = QLabel(self)  # 創建一個 QLabel
        self.label.setFont(QFont("Times", 20, QFont.Bold))  # 設定字體為 Times，大小為20，並加粗
        self.label.setStyleSheet("color: black; background-color: white; border: 0px") # 設定字體顏色為白色，背景顏色為黑色
        self.label.setAlignment(Qt.AlignCenter)
        self.label.setContentsMargins(0, 0, 0, 20)  # 左、上、右、下 讓字稍微往上一點
        self.label.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)  # 設定 QLabel 的 sizePolicy 為 Expanding

        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_figure)
        self.timer.start(400) # 也是要調的東西 0.5其實還是很快 目前是調到1500， 如果調小的話連帶上面的frame、interval 都要試

        gaugeLayout = QVBoxLayout()  # 創建一個 QVBoxLayout
        gaugeLayout.setSpacing(0)  # 設定部件間的空白為0
        gaugeLayout.setContentsMargins(0, 0, 0, 0)  # 設定邊緣間距為0
        gaugeLayout.addWidget(self.gauge)
        gaugeLayout.addWidget(self.label)

        # 製造相連的效果
        layoutWidget = QWidget(self)
        layoutWidget.setLayout(gaugeLayout)
        layoutWidget.setStyleSheet("background-color: white") # 將 QWidget 的背景顏色設為黑色 (讓儀表指針跟prediticon字樣相連)

        layout = QHBoxLayout()
        layout.addWidget(self.canvas)
        # layout.addLayout(gaugeLayout)  # 將 QVBoxLayout 添加到 QHBoxLayout 中
        layout.addWidget(layoutWidget)  # 將包含 gauge 和 label 的 QWidget 添加到 QHBoxLayout 中
        self.setLayout(layout)

    def update_figure(self):
        # val = random.choice(range(60)) * 6  # 0~354 # 測是隨機性
        # self.val = (self.val + 180)%360 # 測試極端值變化，很吃電腦性能，第一次跑會正常，第二次跑會有點誤差，每次跑都需要重新啟動kernal
        # self.val = (self.val + 60)%360 # 測試變化動畫
        client, addr = s.accept()
        while True:
            data = client.recv(1024)
            if len(data) == 0:
                break
            else:
                try:
                    # print(data.decode())
                    pred = int(data.decode())
                    val = pred * scaling
                    map_degree = (pred*scaling + 360 - init_degree) % 360
                    gauge_val = round((map_degree*value_per_scale) / degree_per_scale, 2)

                    if gauge_val > warning_threshold:
                        self.label.setText(f"Predict Degree: {val}, Gauge Value: {gauge_val}, State: Warning!")
                        self.label.setStyleSheet("color:red")
                    else:    
                        self.label.setText(f"Predict Degree: {val}, Gauge Value: {gauge_val}, State: Normal")  # 更新 QLabel 的文字
                        self.label.setStyleSheet("color:black")

                    self.canvas.plot(gauge_val) # plot value
                    print(gauge_val)
                    self.gauge.plot(val)
                    
                except:
                    pass

        client.close()   

        # val = self.val
        # self.canvas.plot(val)
        # self.gauge.plot(val)
        # self.label.setText(f"Prediction: {val}")  # 更新 QLabel 的文字
        
if __name__ == '__main__':
    init_degree = 215
    scaling = 5
    degree_per_scale = 7.2
    value_per_scale = 0.1
    map_degree = init_degree
    gauge_val = 0

    warning_threshold = 1.0

    ip = '172.20.10.5'  #'172.20.10.2'
    port = 8888

    s = socket.socket()
    s.bind((ip, port))
    s.listen(0)
    app = QApplication(sys.argv)

    main = ApplicationWindow()
    main.show()

    sys.exit(app.exec_())
