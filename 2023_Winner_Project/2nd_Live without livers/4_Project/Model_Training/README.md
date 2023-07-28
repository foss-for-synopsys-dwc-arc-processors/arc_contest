# Distyolo on yolov7

Use image to predict the distance with modified structure of yolov7

## Reference thesis:
https://www.mdpi.com/2076-3417/12/3/1354

## Environment:
*   torch 1.8.0
*   torchvision 0.9.0
*   python 3.6.9

## Training
Data preparation
*   Kitty datasets : wait for upload
*   Nuscenes datasets : wait for upload
Note : the distance label for Kitty dataset is normalize with 150

## Training
```bash
    # Nusecenes datasets
    python train.py --workers 8 --epochs 600 --device 0 --batch-size 64 --data nuscenes.yaml --img 640 640 --cfg cfg/training/yolov7-tiny.yaml --weights './yolov7-tiny.pt' --name yolov7-dist-nuscenes --hyp data/hyp.scratch.tiny.yaml 
    # Kitty datasets
    python train.py --workers 8 --epochs 600 --device 0 --batch-size 64 --data kitty.yaml --img 640 640 --cfg cfg/training/yolov7-tiny.yaml --weights './yolov7-tiny.pt' --name yolov7-dist-nuscenes --hyp data/hyp.scratch.tiny.yaml 
```

## Testing
```bash
    # pytorch model testing
    python pytorch_test.py --weights best.pt --source ./nuscenes/val.txt --img-size 640 --stat-path runs/nuscenes/torch/

    # tensorflow lite model testing
    python tflm_test.py --weights tflm_model/arc_model_30_v2_integer_quant.tflite --source arc_nuscenes/val.txt
```

# video inference
```bash
    python video_detect.py --weights best.pt --video test.MOV
```

# pytorch -> onnx
see convert-onnx-step.txt

## onnx -> tensorrt
```bash
    # run on jetson-nano
    $ source onnx2trt.sh best-nms.onnx best.trt
```