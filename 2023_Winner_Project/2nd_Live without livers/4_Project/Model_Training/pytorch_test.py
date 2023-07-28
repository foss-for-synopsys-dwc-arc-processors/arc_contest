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

from my_testing import MyTester

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

def LoadImage(path, img_size, stride):
    img0 = cv2.imread(path)
    # img0 = cv2.cvtColor(img0, cv2.COLOR_BGR2GRAY)
    img = letterbox(img0, stride=stride)[0]

    # Convert
    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    img = [img]
    # img = img[:, :, ::-1].transpose(2, 0, 1)  # BGR to RGB
    # img = np.ascontiguousarray(img)
    return path, img, img0

def rescale(list, X, Y):
    x = float(list[0])*X
    y = float(list[1])*Y
    w = float(list[2])*X
    h = float(list[3])*Y

    x1 = int(x - w/2)
    y1 = int(y - h/2)
    x2 = int(x + w/2)
    y2 = int(y + h/2)
    return [x1, y1, x2, y2]

def iou(bbox1, bbox2):
    ax1 = bbox1[0]
    ax2 = bbox1[2]
    ay1 = bbox1[1]
    ay2 = bbox1[3]
    bx1 = bbox2[0]
    bx2 = bbox2[2]
    by1 = bbox2[1]
    by2 = bbox2[3]

    overlap_w = min(ax2,bx2) - max(ax1,bx1)
    overlap_h = min(ay2,by2) - max(ay1,by1)
    overlap_area = overlap_w*overlap_h
    a_area = (bbox1[2] - bbox1[0]) * (bbox1[3] - bbox1[1])
    b_area = (bbox2[2] - bbox2[0]) * (bbox2[3] - bbox2[1])
    ab_union = a_area+b_area-overlap_area
    if ab_union == 0:
        return 0
    return float(overlap_area)/float(ab_union)


def detect(save_img=False):
    source, weights, imgsz = opt.source, opt.weights, opt.img_size


    # save directory
    # save_dir = Path(increment_path(Path(opt.project) / opt.name, exist_ok=False))
    save_dir = Path(Path(opt.project) / opt.name)
    (save_dir).mkdir(parents=True, exist_ok=True)

    # Load model
    device = torch.device('cuda:0') if opt.device != 'cpu' else torch.device('cpu')
    half = device.type != 'cpu'
    ckpt = torch.load(weights[0], map_location=device)
    model = ckpt['ema' if ckpt.get('ema') else 'model'].float().fuse()
    model.eval()
    stride = int(model.stride.max())

    # trace model
    model = TracedModel(model, device, opt.img_size)
    
    # Get names and colors
    names = model.module.names if hasattr(model, 'module') else model.names
    colors = [[random.randint(0, 255) for _ in range(3)] for _ in names]

    # to FP16
    if half:
        model.half()


    if source.endswith(".png"):
        print("ask Cheng how to use this program")
        exit()

    file = open(source, 'r')

    l = source.split('/')[:-1]
    base = ""
    for t in l:
        base = base + (t + '/')


    # Initialize for statistic
    # dist_size = [0 for _ in range(15)]
    total_target = 0
    target_per_dist = np.array([0 for _ in range(200)])
    total_error = 0
    error_per_dist = np.array([0 for _ in range(200)], dtype=float)
    error_per_cls = np.array([0 for _ in names], dtype=float)
    target_per_cls = np.array([0 for _ in names])

    total_image = 0
    total_time = 0
    # Start process per image
    img_idx = 0

    # for tester
    all_gt = []
    all_pred = []
    for line in file:

        img_path = base + line[2:].replace("\n", "")
        label_path = img_path.replace("images", "labels")[:-3] + "txt"

        if not os.path.isfile(img_path) or not os.path.isfile(label_path):
            print("label or image missing")
            print(f"name : {line}")

        # Load image    
        path, img, img0 = LoadImage(img_path, imgsz, stride)
        img = torch.from_numpy(np.array([img])).to(device)
        img = img.half() # uint8 to fp16/32
        img /= 255.0  # 0 - 255 to 0.0 - 1.0

        # Inference
        t1 = time_synchronized()
        with torch.no_grad():   # Calculating gradients would cause a GPU memory leak
            pred = model(img)[0]

        # NMS
        t2 = time_synchronized()
        pred = non_max_suppression(pred, opt.conf_thres, opt.iou_thres)
        t3 = time_synchronized()

        # Load ground truth
        # (cls, x1, y1, x2, y2, dist)
        gt = []
        X = img0.shape[1]
        Y = img0.shape[0]

        with open(label_path, 'r') as f:
            for line in f:
                list = line.split(' ')
                list[0] = int(list[0])
                tcls = list[0]
                # Rescale bbox and change from xywh2xyxy
                list[0:4] = rescale(list[1:5], X, Y)

                # Rescale dist
                list[5] = float(list[5]) * 30
                # list[5] = float(list[5])
                # list[5] *= 150
                list[4] = tcls
                gt.append(list)
        all_gt.append(np.array(gt))
        
        pred_test = []
        # Process detections
        for i, det in enumerate(pred):
            p = Path(img_path)
            save_path = str(save_dir / p.name)
            # Rescale boxes from img_size to img0 size
            det[:, :4] = scale_coords(img.shape[2:], det[:, :4], img0.shape).round()
            det[:, 5] *= 30
            for tmp in det:
                pred_test.append(tmp.detach().cpu().numpy())
            continue
            # Print result on image and save
            for *xyxy, conf, dist, cls in reversed(det):
                # Rescale dist
                # dist = dist*150
                label = f'{names[int(cls)]} conf:{conf:.2f} dist:{dist:.2f}m'
                plot_one_box(xyxy, img0, label=label, color=colors[int(cls)], line_thickness=1)
                # dist = dist.cpu() * 100
                # Statistic with ground truth
                for l in gt:
                    if iou(xyxy, l[1:5]) > 0.45 and int(cls) == int(l[0]):
                        # print("found")
                        total_target += 1
                        total_error += abs(l[5] - dist)
                        error_per_dist[int(l[5] / 1)] += abs(l[5] - dist)
                        target_per_dist[int(l[5] / 1)] += 1
                        error_per_cls[int(cls)] += abs(l[5] - dist)
                        target_per_cls[int(cls)] += 1
        all_pred.append(np.array(pred_test))
        # cv2.imwrite(save_path, img0)
        # print(f"image save in : {save_path}")
        print(f'Done. ({(1E3 * (t2 - t1)):.1f}ms) Inference, ({(1E3 * (t3 - t2)):.1f}ms) NMS')
        
        # ignore first 10 image for warmup
        if img_idx > 10:
            total_image += 1
            total_time += t3 - t1
            if img_idx > 500:
                break
        img_idx += 1

    tester = MyTester(all_labels=np.array(all_gt), all_predictions=np.array(all_pred))
    tester.showResult()
    print(f'Done. ({(1E3 * (total_time/total_image)):.1f}ms) average time Inference and NMS')
    return 0
    # Plot result
    total_error /= total_target
    error_per_dist /= target_per_dist
    error_per_cls /= target_per_cls

    s_path = opt.stat_path
    # 
    plt.figure(figsize=(15,3))
    plt.bar(names, error_per_cls)
    plt.savefig(s_path + "error-cls.png")
    plt.clf()
    #
    plt.figure(figsize=(15,3))
    plt.bar([i*1 for i in range(200)], error_per_dist)
    plt.savefig(s_path + "error-dist-200.png")
    plt.clf()
    #
    plt.figure(figsize=(15,3))
    plt.bar([i*1 for i in range(200)], target_per_dist)
    plt.savefig(s_path + "target-dist-200.png")

    print(f"total average error : {total_error}")
    print(f'Done. ({(1E3 * (total_time)):.1f}ms) total Inference and NMS')
    print(f'Done. ({(1E3 * (total_time/total_image)):.1f}ms) average time Inference and NMS')

    # save on txt
    with open(s_path + 'summry.txt', 'w') as f:
        f.write(f'target per dist:\n{target_per_dist}\n')
        f.write(f'error per dist:\n{error_per_dist}\n')
        f.write(f'target per cls:\n{target_per_cls}\n')
        f.write(f'error per cls:\n{error_per_cls}\n')
        f.write(f'average error:\n{total_error}\n')
        f.write(f'average runtime:\n{(1E3 * (total_time/total_image)):.1f}ms')
    exit()

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--weights', nargs='+', type=str, help='model.pt path')
    parser.add_argument('--source', type=str, help='input image')
    parser.add_argument('--img-size', type=int, default=640, help='image size')
    parser.add_argument('--conf-thres', type=float, default=0.25)
    parser.add_argument('--iou-thres', type=float, default=0.45)
    parser.add_argument('--device', default='0')
    parser.add_argument('--project', default='runs/detect')
    parser.add_argument('--name', default='exp')
    parser.add_argument('--stat-path', help='statistic save path (end with /)')
    opt = parser.parse_args()
    print(opt)

    if not os.path.exists(opt.stat_path):
        os.mkdir(opt.stat_path)

    detect()