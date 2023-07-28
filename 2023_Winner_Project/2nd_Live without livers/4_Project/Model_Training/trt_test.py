import tensorrt as trt
import torch
import argparse
import os
import sys
import numpy as np
import random
from pathlib import Path
import os.path
import matplotlib.pyplot as plt
import cv2
import time

import pycuda.driver as cuda
import pycuda.autoinit

from utils.general import check_img_size, check_requirements, check_imshow, non_max_suppression, apply_classifier, \
    scale_coords, xyxy2xywh, xywh2xyxy, strip_optimizer, set_logging, increment_path
from utils.plots import plot_one_box
from utils.torch_utils import time_synchronized

from yolo2onnx import LoadImage
from pytorch_test import rescale, iou
# Reference 
# https://docs.nvidia.com/deeplearning/tensorrt/developer-guide/index.html#build_engine_python
# https://blog.csdn.net/irving512/article/details/115403888
# https://github.com/dkorobchenko/tensorrt-demo/blob/master/trt_infer.py
def trt_detect():

    # Load trt model
    logger = trt.Logger(trt.Logger.INFO)
    trt.init_libnvinfer_plugins(logger, namespace="")
    with open(opt.weights[0], 'rb') as f, trt.Runtime(logger) as runtime:
        engine = runtime.deserialize_cuda_engine(f.read())
   
    
    # Print model info
    model_all_names = []
    if opt.show_model:
        for idx in range(engine.num_bindings):
            is_input = engine.binding_is_input(idx)
            name = engine.get_binding_name(idx)
            op_type = engine.get_binding_dtype(idx)
            model_all_names.append(name)
            shape = engine.get_binding_shape(idx)
            print(f'input id:{idx} is input: {is_input} binding name :{name} shape:{shape} type {op_type}')

    # model info
    # input id:0 is input: True binding name :images shape:(1, 3, 224, 640) type DataType.FLOAT
    # input id:1 is input: False binding name :output shape:(100, 7) type DataType.FLOAT
    
    # Prepare TRT execution context, CUDA stream and necessary buffer
    context = engine.create_execution_context()
    stream = cuda.Stream()
    host_in = cuda.pagelocked_empty(trt.volume(engine.get_binding_shape(0)), dtype=opt.datatype)
    host_out = cuda.pagelocked_empty(trt.volume(engine.get_binding_shape(1)), dtype=opt.datatype)
    devide_in = cuda.mem_alloc(host_in.nbytes)
    devide_out = cuda.mem_alloc(host_out.nbytes)

    # save directory
    # save_dir = Path(increment_path(Path(opt.project) / opt.name, exist_ok=False))
    save_dir = Path(Path(opt.project) / opt.name)
    (save_dir).mkdir(parents=True, exist_ok=True)

    # names and color
    if opt.source.replace('val.txt', '') == './nuscenes/':
        print('nuscenes')
        names = [ 'pedestrian', 'animal', 'car', 'motorcycle', 'bicycle', 'bus', 'truck', 'construction', 'emergency', 'trailer', 'barrier', 'trafficcone', 'pushable_pullable', 'debris', 'bicycle_rack']
    else:
        names = [ 'person', 'car', 'van', 'truck', 'person-sit', 'bcycle', 'train']
    colors = [[random.randint(0, 255) for _ in range(3)] for _ in names]

    # process file name
    source = opt.source
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
    for line in file:

        img_path = base + line[2:].replace("\n", "")
        label_path = img_path.replace("images", "labels")[:-3] + "txt"

        if not os.path.isfile(img_path) or not os.path.isfile(label_path):
            print("label or image missing")
            print(f"name : {line}")

        # Load image
        path, img, img0 = LoadImage(img_path, opt.img_size, 32)
        # Only for cuda:0 because is running on jetson nano
        #img = torch.from_numpy(np.array([img])).to('cuda:0')
        # unnecessary convert to cuda and torch using numpy is fine
        img = np.array([img], dtype=opt.datatype) # uint8 to fp16/32
        img /= 255.0  # 0 - 255 to 0.0 - 1.0

        # Inferece
        t1 = time.time()
        bindings = [int(devide_in), int(devide_out)]
        np.copyto(host_in, img.ravel())
        cuda.memcpy_htod_async(devide_in, host_in, stream)
        context.execute_async(bindings=bindings, stream_handle=stream.handle)
        cuda.memcpy_dtoh_async(host_out, devide_out, stream)
        stream.synchronize()

        # reshape and remove duplicate
        pred = np.unique(host_out.reshape(engine.get_binding_shape(1)), axis=0)
        print(pred)
        t2 = time.time()

        # Load ground truth
        # (cls, x1, y1, x2, y2, dist)
        gt = []
        X = img0.shape[1]
        Y = img0.shape[0]

        with open(label_path, 'r') as f:
            for line in f:
                list = line.split(' ')
                list[0] = int(list[0])
                # Rescale bbox and change from xywh2xyxy
                list[1:5] = rescale(list[1:5], X, Y)

                # Rescale dist
                list[5] = float(list[5])
                # list[5] *= 150
                gt.append(list)

        # Process detections
        p = Path(img_path)
        save_path = str(save_dir / p.name)
        
        # Rescale boxes from img_size to img0 size
        det = torch.from_numpy(pred).float()
        det[:, :4] = scale_coords(img.shape[2:], det[:, :4], img0.shape).round()

        # Print result on image and save
        for *xyxy, conf, dist, cls in reversed(det):
            print(img_path)
            label = f'{names[int(cls)]} conf:{conf:.2f} dist:{dist:.2f}m'
            plot_one_box(xyxy, img0, label=label, color=colors[int(cls)], line_thickness=1)
            dist = dist.cpu()
            # Statistic with ground truth
            for l in gt:
                if iou(xyxy, l[1:5]) > 0.5 and int(cls) == int(l[0]):
                    # print("found")
                    total_target += 1
                    total_error += abs(l[5] - dist)
                    error_per_dist[int(l[5] / 1)] += abs(l[5] - dist)
                    target_per_dist[int(l[5] / 1)] += 1
                    error_per_cls[int(cls)] += abs(l[5] - dist)
                    target_per_cls[int(cls)] += 1
        cv2.imwrite(save_path, img0)
        print(f"image save in : {save_path}")
        print(f'Done. ({(1E3 * (t2 - t1)):.1f}ms) Inference')
        
        # ignore first 10 image for warmup
        if img_idx > 10:
            total_image += 1
            total_time += t2 - t1

        img_idx += 1
   
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


if __name__ == '__main__':
    print('This test program is for trt model with nms becareful!!!!!!!!!')
    parser = argparse.ArgumentParser()
    parser.add_argument('--weights', nargs='+', type=str, help='model.trt path')
    parser.add_argument('--source', default='', help='input set path')
    parser.add_argument('--img_size', default=640)
    parser.add_argument('--show-model', action='store_true', help='print details of trt model')
    parser.add_argument('--datatype', default=float)
    parser.add_argument('--project', default='runs/detect')
    parser.add_argument('--name', default='exp-trt')
    parser.add_argument('--stat-path', help='statistic save path (end with /)')
    opt = parser.parse_args()
    # opt.datatype = np.float32
    print(opt)

    if not os.path.exists(opt.stat_path):
        os.mkdir(opt.stat_path)

    trt_detect()