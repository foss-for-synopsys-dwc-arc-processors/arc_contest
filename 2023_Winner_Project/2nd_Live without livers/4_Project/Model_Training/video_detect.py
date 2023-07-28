import argparse
from pathlib import Path
import os.path


import torch
import cv2
import numpy as np
import sys
import random
import matplotlib.pyplot as plt

from models.experimental import attempt_load
from utils.datasets import LoadStreams, LoadImages
from utils.general import check_img_size, check_requirements, check_imshow, non_max_suppression, apply_classifier, \
    scale_coords, xyxy2xywh, xywh2xyxy, strip_optimizer, set_logging, increment_path
from utils.plots import plot_one_box
from utils.torch_utils import select_device, load_classifier, time_synchronized, TracedModel



def letterbox(img, new_shape=(640, 640), color=(114, 114, 114), auto=True, scaleFill=False, scaleup=True, stride=32):
    # Resize and pad image while meeting stride-multiple constraints
    shape = img.shape[:2]  # current shape [height, width]
    if isinstance(new_shape, int):
        new_shape = (new_shape, new_shape)

    # Scale ratio (new / old)
    r = min(new_shape[0] / shape[0], new_shape[1] / shape[1])
    if not scaleup:  # only scale down, do not scale up (for better test mAP)
        r = min(r, 1.0)

    # Compute padding
    ratio = r, r  # width, height ratios
    new_unpad = int(round(shape[1] * r)), int(round(shape[0] * r))
    dw, dh = new_shape[1] - new_unpad[0], new_shape[0] - new_unpad[1]  # wh padding
    if auto:  # minimum rectangle
        dw, dh = np.mod(dw, stride), np.mod(dh, stride)  # wh padding
    elif scaleFill:  # stretch
        dw, dh = 0.0, 0.0
        new_unpad = (new_shape[1], new_shape[0])
        ratio = new_shape[1] / shape[1], new_shape[0] / shape[0]  # width, height ratios

    dw /= 2  # divide padding into 2 sides
    dh /= 2

    if shape[::-1] != new_unpad:  # resize
        img = cv2.resize(img, new_unpad, interpolation=cv2.INTER_LINEAR)
    top, bottom = int(round(dh - 0.1)), int(round(dh + 0.1))
    left, right = int(round(dw - 0.1)), int(round(dw + 0.1))
    img = cv2.copyMakeBorder(img, top, bottom, left, right, cv2.BORDER_CONSTANT, value=color)  # add border
    return img, ratio, (dw, dh)

def detect():
    source, weights, imgsz = opt.video, opt.weights[0], opt.img_size


    save_dir = Path(opt.save)
    (save_dir).mkdir(parents=True, exist_ok=True)

    # Load model
    device = torch.device('cuda:0') if opt.device != 'cpu' else torch.device('cpu')
    half = device.type != 'cpu'
    ckpt = torch.load(weights, map_location=device)
    model = ckpt['ema' if ckpt.get('ema') else 'model'].float().fuse()
    model.eval()
    stride = int(model.stride.max())

    # trace model
    model = TracedModel(model, device, opt.img_size)
    

    # Get names and colors
    names = model.module.names if hasattr(model, 'module') else model.names
    colors = [[random.randint(0, 255) for _ in range(3)] for _ in names]

    # read video
    cap = cv2.VideoCapture(source)
    width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    fps = int(round(cap.get(cv2.CAP_PROP_FPS)))
    print(fps, width, height)
    fourcc = cv2.VideoWriter_fourcc(*'MPEG')
    out = cv2.VideoWriter(str(save_dir / 'out_letterbox.mp4'), fourcc, fps, (width, height))
    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            print('finish')
            break
        # print(frame.shape)
        frame0 = frame
        frame = letterbox(frame0, stride=32, new_shape=(320, 320))[0]
        # frame = cv2.resize(frame0, (320,320))
        # change transpose to cvtColor for gray image and add another shape
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        #　frame = frame.transpose(2, 0, 1)
        frame = torch.from_numpy(np.array([[frame]])).to(device)
        frame = frame.float()
        frame /= 255.0
        # Inference
        t1 = time_synchronized()
        with torch.no_grad():   # Calculating gradients would cause a GPU memory leak
            pred = model(frame)[0]

        # NMS
        t2 = time_synchronized()
        pred = non_max_suppression(pred, opt.conf_thres, opt.iou_thres)
        t3 = time_synchronized()

        # Process detections
        for i, det in enumerate(pred):
            det[:, :4] = scale_coords(frame.shape[2:], det[:, :4], frame0.shape).round()

            # Print result on image and save
            for *xyxy, conf, dist, cls in reversed(det):
                # Rescale dist
                # dist = dist*150

                label = f'{names[int(cls)]} conf:{conf:.2f} dist:{dist:.2f}m'
                plot_one_box(xyxy, frame0, label=label, color=colors[int(cls)], line_thickness=3)
                dist = dist.cpu()
        # print(frame0.shape)
        out.write(frame0)
        
    cap.release()
    exit()







if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--weights', nargs='+', type=str)
    parser.add_argument('--video', type=str)

    parser.add_argument('--img-size', type=int, default=640, help='image size')
    parser.add_argument('--conf-thres', type=float, default=0.25)
    parser.add_argument('--iou-thres', type=float, default=0.35)
    parser.add_argument('--device', default='0')
    parser.add_argument('--save', default='video/')

    opt = parser.parse_args()
    print(opt)

    detect()