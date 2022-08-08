# 2022 ARC AIoT contest
作品名稱: 跑步訓練監控平台

組名: 仰望星空

組員: 楊博禕 林子華
## Introduction
步態時空參數是提升運動表現的關鍵，跑步著地指數是指跑步過程中，腳著地時間佔整個步態週期的百分比，教練可以依照不同的著地指數訂定合適的訓練計劃。

本作品旨在提供一套完整的跑步監測系統，幫助教練分析跑者的步態。
## HW/SW Setup
HW: ARC AIoT EM9D DK + nRF52840 + MPU9250

SW: Synopsys SDK v22.01 + nRF52 SDK17.1.0 + monitor_and_parse.py

## User manual 
* Step 00
    nRF52840上有五顆led燈: 1代表亮 0代表暗

    狀態10000: setup finish

    狀態01000: Start hint timer(進入熱身倒數15sec)

    狀態00100: hint timer timeout. Start to sample

    狀態00010: sample end. Ready to send UART signal

    狀態00001: UART sending.

    狀態11111: UART sending finish. Set GPIO6 to low.
* Step 01
    按下Button 01，開始跑步
* Step 02
    直到燈號變成00010，將nRF52840與EM9D DK接上(需要Tx Rx GPIO6 GND四條線)
* Step 03
    同時在pc上跑monitor.py，監聽COM Port的訊息
* Step 04
    最後，按下Button 02開始UART傳輸
* Step 05
    觀察PC上的步態時空參數與原始三軸加速度資料

## Demo Link
https://www.youtube.com/watch?v=tPOsxX0Oops
