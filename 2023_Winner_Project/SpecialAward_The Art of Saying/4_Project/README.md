# 2023 ARC AIoT Design Contest - 好好說話

## Introduction
### Motivation

傳統的打卡系統需要使用卡片或密碼進行身份驗證，使用者一旦遺忘密碼、遺失卡片，甚至被盜用，就會面臨諸多麻煩和不便。考量到進幾年新冠疫情的影響，若使用接觸式的身體特徵(如指紋)來進行身份驗證，則可能提高病菌傳播的風險。透過每個人獨特的聲音特徵來進行身份驗證的聲紋辨識技術，沒有上述風險，且能大幅提高安全性和可靠性。聲紋辨識技術具有零接觸、便捷、環保、難以冒充...等優點，鑑於此，決定將這項技術應用於打卡系統，提供使用者更好的體驗。

### Design

聲紋打卡系統包含本地端在微控制器上的關鍵字偵測系統與雲端的身份驗證系統。
- 本地端使用微控制器 ARC EM9D AIoT DK，並運行關鍵詞偵測 (Keyword Spotting) 模型。
- 雲端使用 Google Cloud Platform (GCP)，並運行聲紋辨識 (Voiceprint Recognition) 模型與資料庫紀錄合法使用者的聲紋特徵、紀錄出缺勤狀況。

ARC EM9D AIoT DK 會透過麥克風即時收集環境音，當偵測出關鍵字時，會將所收到的音訊傳送至雲端，利用聲紋辨識技術對說話者進行身分驗證，更新資料庫中的出缺勤狀況，並回傳辨識結果，將其用於判斷是否解鎖。

為提升系統之高效性與增加整體收音比率，在模型架構、推論延遲與收音方式三個面向進行優化：
* 在模型架構方面，除了將模型縮小，也採用了 Google KWS streaming  model 取代原先的 non-stream model，以將運算量大幅減少。此外，streaming model 的模型輸入較短，這也利於降低音訊中斷時間與縮短推論時間。
* 在減少延遲方面，有以下兩項優化
    * 將移植的運算子內各個 window 之初始化從 TFLM Eval 階段移到 Prepare 階段，並把運算子內動態宣告的容器改成靜態陣列，於 PersistentBuffer 時一起宣告，這使得在 Prepare 階段運算的結果得以保存，每次推論時不須重複計算。此項優化將延遲減少約 121ms。
    * 使用 MLI library 所提供的 Depthwise_Conv2D 與 Fully_Connected 運算子，加速模型運行。
* 在收音方面，我們將原先 CPU 忙碌等待的收音方式，改為同時進行收音及推論，待推論完成後再等待並保存收音結果。

結合以上優化，成功地將收音比率從 42% 提高至約 70 %，並將單次推論延遲時間降低 97% ~ 98%，即只須耗費原本 2 ~ 3% 的時間就可完成一次推論，使關鍵詞模型在 ARC EM9D 開發板上能以 always-on 的方式更高效得運行。

## HW/SW Setup

### 本地端(EM9D)
#### 安裝指南

1. 進入專案資料夾: Synopsys_SDK_V24_Willie_Shared，並複製2023_ARC_final_work-art_of_saying.zip到專案根目錄。
```shell!
$ SDK_ROOT_PATH=Synopsys_SDK_V24_Willie_Shared
$ cd $(SDK_ROOT_PATH)
$ cp ~/Downloads/2023_ARC_final_work-art_of_saying.zip .
```
2. 解壓縮2023_ARC_final_work-art_of_saying.zip
```shell!
$ unzip 2023_ARC_final_work-art_of_saying.zip
```
解開分為兩個部分：專案資料夾(kws_project/aiot_contest_stream/)與TFLM 運算子修改的補丁檔(2023_ARC_final_work-art_of_saying-kws_tflm.patch)，檔案清單如下:
```
kws_project/aiot_contest_stream/makefile
kws_project/aiot_contest_stream/app.mk
kws_project/aiot_contest_stream/arc_get_audio_by_serial.py
kws_project/aiot_contest_stream/inc/main.h
kws_project/aiot_contest_stream/inc/model.h
kws_project/aiot_contest_stream/inc/model_settings.h
kws_project/aiot_contest_stream/inc/tflitemicro_algo.h
kws_project/aiot_contest_stream/src/model_settings.cpp
kws_project/aiot_contest_stream/src/main.c
kws_project/aiot_contest_stream/src/tflitemicro_algo.cpp
2023_ARC_final_work-art_of_saying-kws_tflm.patch
```
3. 透過補丁檔安裝額外的TFLM 運算子 (MFCC、Audio_spectrogram)
```shell!
$ patch -p1 -i 2023_ARC_final_work-art_of_saying-kws_tflm.patch
```
如下檔案被修改或新增：(修改內容詳見補丁檔)
```shell!
library/cv/tflitemicro_24/tensorflow/lite/kernels/internal/mfcc_dct.cc
library/cv/tflitemicro_24/tensorflow/lite/kernels/internal/mfcc_dct.h
library/cv/tflitemicro_24/tensorflow/lite/kernels/internal/mfcc_function.cc
library/cv/tflitemicro_24/tensorflow/lite/kernels/internal/mfcc.h
library/cv/tflitemicro_24/tensorflow/lite/kernels/internal/mfcc_mel_filterbank.cc
library/cv/tflitemicro_24/tensorflow/lite/kernels/internal/mfcc_mel_filterbank.h
library/cv/tflitemicro_24/tensorflow/lite/kernels/internal/spectrogram.cc
library/cv/tflitemicro_24/tensorflow/lite/kernels/internal/spectrogram.h
library/cv/tflitemicro_24/tensorflow/lite/kernels/internal/tensor.h
library/cv/tflitemicro_24/tensorflow/lite/micro/kernels/arc_mli/depthwise_conv.cc
library/cv/tflitemicro_24/tensorflow/lite/micro/kernels/audio_spectrogram.cc
library/cv/tflitemicro_24/tensorflow/lite/micro/kernels/mfcc.cc
library/cv/tflitemicro_24/tensorflow/lite/micro/kernels/micro_ops.h
library/cv/tflitemicro_24/tensorflow/lite/micro/kernels/reduce.cc
library/cv/tflitemicro_24/tensorflow/lite/micro/micro_mutable_op_resolver.h
library/cv/tflitemicro_24/tensorflow/lite/string_type.h
library/cv/tflitemicro_24/tensorflow/lite/string_util.cc
library/cv/tflitemicro_24/tensorflow/lite/string_util.h
library/cv/tflitemicro_24/tflitemicro_24.mk
library/cv/tflitemicro_24/third_party/fft2d/fft4g.c
library/cv/tflitemicro_24/third_party/fft2d/fft4g_h.c
library/cv/tflitemicro_24/third_party/fft2d/fft8g.c
library/cv/tflitemicro_24/third_party/fft2d/fft8g_h.c
library/cv/tflitemicro_24/third_party/fft2d/fft.h
library/cv/tflitemicro_24/third_party/fft2d/fftsg.c
library/cv/tflitemicro_24/third_party/fft2d/fftsg_h.c
library/cv/tflitemicro_24/third_party/fft2d/LICENSE
library/cv/tflitemicro_24/third_party/fft2d/readme.txt
library/cv/tflitemicro_24/third_party/flatbuffers/include/flatbuffers/flexbuffers.h
library/cv/tflitemicro_24/third_party/flatbuffers/include/flatbuffers/util.h
```
4. 進入 aiot_contest_stream 資料夾並編譯專案
```shell!
$ cd kws_project/aiot_contest_stream/
$ make
$ make flash
```
預設是沒有使用 CV pre-build library，可編譯一次之後，將 libtflitemicro_24.a 取代預設函式庫，並修改 makefile 相關參數即可縮短下次專案的編譯時間。
```shell!
$ cp obj_socket_24/gnu_arcem9d_wei_r16/libtflitemicro_24.a ../../library/cv/tflitemicro_24/prebuilt_lib/libtflitemicro_google_person_gnu.a
$ sed -i 's/LIB_CV_PREBUILT = 0/LIB_CV_PREBUILT = 1/' makefile
```
若有再修改 TFLM 運算子，則需要重新編譯函式庫。
5. 使用 HMX_FT4222H_GUI.exe 燒入程式
6. 安裝電腦 python 執行環境：
    a. 參照 `Tutorial-3_TensorFlow Project Environment Setup Development Flow_2023_0206.pdf` 安裝 anaconda 虛擬環境
    b. 安裝額外函式庫
```shell
$ pip install pyserial
```
7. 執行 ARC EM9D 開發版後，執行 arc_get_audio_by_serial.py，系統中的本地端即完成設定並開始運作。
```shell!
$ python arc_get_audio_by_serial.py
```
8. 當偵測到關鍵詞，即傳送資料與雲端互動，取得結果。

### 雲端(GCP)

#### 系統需求
- Python 版本：3.7 或更高版本
- 作業系統：MacOS 、 Linux

#### 安裝指南
1. 複製遠端 [GitHub Repo](https://github.com/WlmWu/synopsys-arc-aiot-2023.git)
```shell!
$ git clone https://github.com/WlmWu/synopsys-arc-aiot-2023.git
```
2. 安裝相關程式庫：
```shell!
$ pip3 install --upgrade pip
$ pip3 --no-cache-dir install -r requirements.txt
```
3. 更改 Authorization Token
```shell!
$ # Replace APP_AUTH_TOKEN with the random string in the dynacloud/config.
$ APP_AUTH_TOKEN=$(openssl rand -hex 40)
$ sed -i "s/APP_AUTH_TOKEN = '.*'/APP_AUTH_TOKEN = '$APP_AUTH_TOKEN'/" ./dynacloud/config.py
```
4. 更改發信帳號之 Google App Password
```python!
# In speaker_recognizion/config.py
GMAIL_APP_PASSWORD = '%_put_your_auth_token_here_%'
```

## User Manual

### 本地端(EM9D)
#### 系統概述
ARC EM9D AIoT DK 會透過麥克風實時收集環境音，當偵測出關鍵字時，將所收到的音訊傳送至雲端。
#### 使用指南
1. 將關鍵詞偵測系統部屬到 ARC EM9D AIoT DK。
2. 將 ARC EM9D AIoT DK 透過序列埠接上電腦，並執行 EM9D。
3. 在電腦上執行序列埠資料接收程式 `arc_get_audio_by_serial.py`，確保程式正確運行後，關鍵詞音訊將開始被傳送至雲端。
注意：須指定正確的序列埠參數 (\-\-com)
```shell!
$ # E.g. 使用序列埠 COM7
$ python arc_get_audio_by_serial.py --com COM7
```

在執行過程中，可以透過 ARC EM9D AIoT DK 上的 LED 燈號來了解當前的執行狀況，燈號意義如下：
* 紅色燈號：系統正在運行中。
* 藍色燈號：已偵測到關鍵詞，並正在將音訊傳送至雲端。
* 綠色燈號：聲紋辨識結果為「pass」，通過驗證。
* 當藍色燈號熄滅，綠色燈號沒亮，表示為「fail」，驗證失敗或沒有權限。


### 雲端(GCP)
#### 系統概述
接收由 EM9D 端傳來之音訊檔案 (.wav)，識別聲紋並替發聲者打卡。
#### 使用指南
##### 執行方式
- 執行 API
```shell!
$ export FLASK_APP=main.py
$ flask run --reload --debugger --host 0.0.0.0 --port 8080
```
- 註冊新員工
```shell!
$ python -m speaker_recognition.recognizer -n path/to/new/audios
```

#### API Docs
可參考 demo.py 之 `send_test_audio()`或執行 demo.py 測試：
```python!
$ python demo.py -f path/to/wav/testing -i your.vm.ip
```
##### 音訊辨識
<details>
  <summary><code>POST</code><code><b>/recognition</b></code></summary>

##### Headers
> | key           | value                  | description                             |
> |---------------|------------------------|-----------------------------------------|
> | Authorization | Bearer $APP_AUTH_TOKEN | Please provide the authorization token. |

##### Parameters
> | name  |  type    | data type  | description             |
> |-------|----------|------------|-------------------------|
> | audio | required | audio file | wav  |

##### Responses
> | http code | content-type        | response            | description |
> |-----------|---------------------|---------------------|-------------|
> | `200`     | `application/json`  | `{"EID": [...]}`   | OK          |
> | `4xx`     | `application/json`  | `{"errors": [...]}` | ClientError |
> | `5xx`     | `application/json`  | `{"errors": [...]}` | ServerError |


##### Example Response
聲音無法識別：
```json!
{
  "EID": "unknown"
}
```
聲音識別成功則為職員ID，如識別為ID 1 號：
```json!
{
  "EID": "1"
}
```
</details>

##### 資料庫瀏覽
<details>
  <summary><code>GET</code><code><b>/database</b></code></summary>

##### Headers
> | key           | value                  | description                             |
> |---------------|------------------------|-----------------------------------------|
> | Authorization | Bearer $APP_AUTH_TOKEN | Please provide the authorization token. |

##### Responses
> | http code | content-type        | response            | description |
> |-----------|---------------------|---------------------|-------------|
> | `200`     | `text/html`  |  index.html  | OK          |
> | `4xx`     | `application/json`  | `{"errors": [...]}` | ClientError |
> | `5xx`     | `application/json`  | `{"errors": [...]}` | ServerError |

</details>

