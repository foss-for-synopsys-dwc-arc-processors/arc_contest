# Shopping Master 購物大師
This is the project design and works for 2022 Synopsys ARC AIoT Design Contest.
#   Introduction
>  系統主要功能包含透過 ARC EM9D AIoT 開發板進行AI人臉辨識，登入會員身分並同時完成實名制登錄後，結合手機APP，讓顧客可以編輯購買備忘清單，上傳至雲端並同步顯示在購物車上的電子紙顯示螢幕供查看，最後透過AI物品辨識和自動結帳的功能，完成無結帳人員接觸並省下排隊的時間。
#  作品照片 Photo
> ### 智慧購物車
>![智慧購物車](./photo/system.png)
> ### 4.2" 電子紙顯示
>![電子紙螢幕](./photo/ePD.jpg)

# 展示影片
>影片展示[網址](https://youtu.be/acdqWC3HPq8)
_ _ _
# 硬體/軟體設定 HW/SW setup
> ### 硬體架構圖 HW Architecture
>![硬體架構圖](./images/HW.png)

>### 程式流程圖-手機APP 購物清單及結帳
>![硬體架構圖](./images/SW1.png)

>### 程式流程圖-AI人臉辨識會員登入
>![硬體架構圖](./images/SW2.png)

>### 程式流程圖-AI物品辨識
>![硬體架構圖](./images/SW3.png)
___
# 操作手冊 User manual

## 準備工具
* 電腦PC, SDK 及 EM9D AIoT DK 
* VirtualBox 軟體
* Tera Term 軟體
* HMX_FT4222H_GUI 燒錄程式

## 燒錄 Flashing the image
1. 程式下載
2. 開啟VirtualBox 執行 make 和 make flash 程式
3. EM9D透過USB連接電腦PC
4. 設定 Jumper, 開啟HMX_FT4222H_GUI 燒入程式 image 至 EM9D 板子
5. 開啟Tera Term
6. 選擇正確的Serial Port(COMx)
7. 更改baud rate 為115200
8. 設定 Jumper 執行程式開始人臉辨識

## 人臉辨識(會員登入)操作流程
>  透過EM9D鏡頭輸入來辨識人臉(會員)，並透過UART將結果傳輸到Node MCU，NodeMCU 連接至AWS然後將RDS MySQL內的購物清單發送回NodeMCU，最後透過Node MCU的另一組 s/w UART 將購物清單資料傳送給E-paper 顯示出來。
>
## 物件辨識(選購)操作流程
>  透過EM9D鏡頭輸入來辨識物件，並透過UART將結果傳輸到Node MCU，透過Node MCU的另一組 s/w UART 將購物清單資料傳送給E-paper 顯示出來, 同時也將購物物件訊息送至 AWS 雲端存放在另一個資料庫內。

