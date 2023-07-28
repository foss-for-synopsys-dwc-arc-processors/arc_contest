# 2023 ARC AIoT Contest - 利用RSSI實現車主認證的藍芽鑰匙

## Introduction
作品名稱: 利用RSSI及加速度實現車主認證的藍芽鑰匙

組名: NEAT team

組員: 張睿凱 徐睿桐 曾浚豪 李韋漢

近來有很多車輛已經開始使用藍芽鑰匙來取代傳統鑰匙。但傳統的藍芽鑰匙，若是只透過手機藍芽MAC address、和車輛交互認證機制進行驗證，仍潛在有被破解、入侵的可能。

本團隊提出，透過藍芽鑰匙端(使用特製藍芽鑰匙)收集車主走路的加速度變化之移動特性來判斷是否為車主本人，與藍芽通訊時RSSI變化的資訊，來判斷是否靠近或遠離汽車。鑰匙端會上述資訊傳輸到車輛端，於車輛端進行machine Learning來驗證鑰匙持有者的身分以及運動狀態，只有在車主身分認證成功以及運動狀態判定為接近時才會解鎖，透過這種方式來進一步提升藍芽鑰匙的安全性。

## HW/SW Setup
HW: ARC AIoT EM9D DK + 兩台ESP32 (以下鑰匙端稱為ESP32-0、車輛端稱為ESP32-1) + MPU9250

SW: Synopsys SDK v24.01 + ESP-idf-v4.4.4

## User Manual
### Step 00
將ESP32-1與EM9D DK之UART接口接上 (ESP32-1 pin4(TX)接到ARC AIoT EM9D DK Arduino UART (RX))，ESP32-1使用IDF環境燒錄ble_ibeacon_scan_0706_uart_send69這份檔案

### Step 01
ESP32-0使用IDF環境燒錄ble_ibeacon_Sender_new這份檔案並連接MPU9250後，將藍芽鑰匙以正面朝外、ESP32電供口朝上放入右側褲子口袋，開始走路 (esp32-0會持續發送beacon)，與EM9D DK相連的車輛端稱為ESP32-1會接收beacon的RSSI以及其中的資訊

### Step 02
資訊會從ESP32-1透過UART傳送至ARC AIoT EM9D DK

### Step 03
資訊經過EM9D DK上的MODEL判斷之後決定是否要開鎖(若要開鎖EM9D亮起藍色及綠色LED表示)

## Demo Link
[YouTube Demo Link](https://youtu.be/djWEkslljew)