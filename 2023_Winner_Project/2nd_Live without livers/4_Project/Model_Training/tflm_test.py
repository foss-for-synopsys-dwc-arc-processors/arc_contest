import tensorflow as tf
import numpy as np
import cv2
import math
import random
import argparse
from pathlib import Path
import time
from my_testing import MyTester
import os

def letterbox(img, new_shape=(640, 640), color=(114, 114, 114), auto=False, scaleFill=False, scaleup=True, stride=32):
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
    img = letterbox(img0, stride=stride, new_shape=(256, 320))[0]

    # Convert
    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    img = np.expand_dims(img, axis=-1)
    # img = [img]
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

def loadImg():
    img = cv2.imread('test.jpg')
    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    img = cv2.resize(img, (320,320))
    img = img.reshape((320,320,1))
    img = img.astype(np.int8)
    return img

def myIdetect(preds, bias, quantize):
    conf_thres = 0.25
    preds = preds[0]

    # preds = preds.transpose(1, 0, 2)

    # bias = -31
    # quantize =  0.10775779187679291

    anchor_grid = [
        [12,23],
		[45,22],
		[80,60]
    ]

    anchor_grid_idx = -1
    gridx = 0
    gridy = 0
    out = []

    for gridy in range(8):
        for gridx in range(10):
            for anchor_grid_idx in range(3):
                # confiden
                conf = (preds[gridy][gridx][anchor_grid_idx*10 + 4] + bias) * quantize
                conf = 1.0 / (1.0 + math.exp(-conf))

                best_conf = 0
                best_cls = -1
                for j in range(4):
                    cls_conf = (preds[gridy][gridx][anchor_grid_idx*10 + 6 + j] + bias) * quantize
                    cls_conf = 1.0 / (1.0 + math.exp(-cls_conf))
                    cls_conf = cls_conf * conf
                    if(cls_conf > conf_thres and cls_conf > best_conf):
                        best_conf = cls_conf
                        best_cls = j

                if(best_conf <= conf_thres):
                    continue

                print(f'detect : {best_cls}, conf = {best_conf}')

                dist = (preds[gridy][gridx][anchor_grid_idx*10 + 5] + bias) * quantize * 30

                # dequantize
                x = (preds[gridy][gridx][anchor_grid_idx*10 + 0] + bias) * quantize
                y = (preds[gridy][gridx][anchor_grid_idx*10 + 1] + bias) * quantize
                w = (preds[gridy][gridx][anchor_grid_idx*10 + 2] + bias) * quantize
                h = (preds[gridy][gridx][anchor_grid_idx*10 + 3] + bias) * quantize

                # sigmoid
                x = 1.0 / (1.0 + math.exp(-x))
                y = 1.0 / (1.0 + math.exp(-y))
                w = 1.0 / (1.0 + math.exp(-w))
                h = 1.0 / (1.0 + math.exp(-h))

                # final result
                x = (x * 2 - 0.5 + gridx) * 32
                y = (y * 2 - 0.5 + gridy) * 32
                w = (w * 2) ** 2 * anchor_grid[anchor_grid_idx][0]
                h = (h * 2) ** 2 * anchor_grid[anchor_grid_idx][1]

                out.append([x, y, w, h, best_conf, dist, best_cls])
    return out

def xywh2xyxy(x):
    # Convert nx4 boxes from [x, y, w, h] to [x1, y1, x2, y2] where xy1=top-left, xy2=bottom-right
    y = np.copy(x)
    y[:, 0] = x[:, 0] - x[:, 2] / 2  # top left x
    y[:, 1] = x[:, 1] - x[:, 3] / 2  # top left y
    y[:, 2] = x[:, 0] + x[:, 2] / 2  # bottom right x
    y[:, 3] = x[:, 1] + x[:, 3] / 2  # bottom right y
    return y

def compute_overlap(box, boxes):
    # Calculate the overlap between a box and a list of boxes
    x1 = np.maximum(box[0], boxes[:, 0])
    y1 = np.maximum(box[1], boxes[:, 1])
    x2 = np.minimum(box[2], boxes[:, 2])
    y2 = np.minimum(box[3], boxes[:, 3])

    intersection = np.maximum(0, x2 - x1) * np.maximum(0, y2 - y1)
    area1 = (box[2] - box[0]) * (box[3] - box[1])
    area2 = (boxes[:, 2] - boxes[:, 0]) * (boxes[:, 3] - boxes[:, 1])
    union = area1 + area2 - intersection

    return intersection / union

def my_nms(pred, iou_thres = 0.45):
    pred = np.array(pred)
    nc = 5
    
    # Settings
    min_wh, max_wh = 2, 4096
    max_det = 300
    max_nms = 3000


    # xywh2xyxy
    pred[:, :4] = xywh2xyxy(pred[:, :4])

    sorted_index = np.argsort(pred[:,4])[::-1]
    pred = pred[sorted_index]

    keep = []
    while pred.shape[0] > 0:
        keep.append(pred[0])
        overlap_index = compute_overlap(pred[0], pred[1:])

        # remove boxes with high overlap
        pred = pred[1:][overlap_index < iou_thres]

    return keep

def clip_coords(boxes, img_shape):
    # Clip bounding xyxy bounding boxes to image shape (height, width)
    boxes[:, 0].clip(0, img_shape[1])  # x1
    boxes[:, 1].clip(0, img_shape[0])  # y1
    boxes[:, 2].clip(0, img_shape[1])  # x2
    boxes[:, 3].clip(0, img_shape[0])  # y2

def scale_coords(img1_shape, coords, img0_shape, ratio_pad=None):
    # Rescale coords (xyxy) from img1_shape to img0_shape
    if ratio_pad is None:  # calculate from img0_shape
        gain = min(img1_shape[0] / img0_shape[0], img1_shape[1] / img0_shape[1])  # gain  = old / new
        pad = (img1_shape[1] - img0_shape[1] * gain) / 2, (img1_shape[0] - img0_shape[0] * gain) / 2  # wh padding
    else:
        gain = ratio_pad[0][0]
        pad = ratio_pad[1]

    coords[:, [0, 2]] -= pad[0]  # x padding
    coords[:, [1, 3]] -= pad[1]  # y padding
    coords[:, :4] /= gain
    clip_coords(coords, img0_shape)
    return coords


def plot_one_box(x, img, color=None, label=None, line_thickness=3):
    # Plots one bounding box on image img
    tl = line_thickness or round(0.002 * (img.shape[0] + img.shape[1]) / 2) + 1  # line/font thickness
    color = color or [random.randint(0, 255) for _ in range(3)]
    c1, c2 = (int(x[0]), int(x[1])), (int(x[2]), int(x[3]))
    cv2.rectangle(img, c1, c2, color, thickness=tl, lineType=cv2.LINE_AA)
    if label:
        tf = max(tl - 1, 1)  # font thickness
        t_size = cv2.getTextSize(label, 0, fontScale=tl / 3, thickness=tf)[0]
        c2 = c1[0] + t_size[0], c1[1] - t_size[1] - 3
        cv2.rectangle(img, c1, c2, color, -1, cv2.LINE_AA)  # filled
        cv2.putText(img, label, (c1[0], c1[1] - 2), 0, tl / 3, [225, 255, 255], thickness=tf, lineType=cv2.LINE_AA)




if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--weights', nargs='+', type=str, help='model.pt path')
    parser.add_argument('--source', type=str, help='input image')
    parser.add_argument('--img-size', type=int, default=640, help='image size')
    parser.add_argument('--conf-thres', type=float, default=0.35)
    parser.add_argument('--iou-thres', type=float, default=0.45)
    parser.add_argument('--device', default='0')
    parser.add_argument('--project', default='runs/detect')
    parser.add_argument('--name', default='arc_int8')
    parser.add_argument('--stat-path', help='statistic save path (end with /)')
    opt = parser.parse_args()


    source, weights, imgsz = opt.source, opt.weights, opt.img_size


    # save directory
    # save_dir = Path(increment_path(Path(opt.project) / opt.name, exist_ok=False))
    save_dir = Path(Path(opt.project) / opt.name)
    (save_dir).mkdir(parents=True, exist_ok=True)

    # Load the TFLite model

    interpreter = tf.lite.Interpreter(model_path=weights[0])
    # interpreter = tf.lite.Interpreter(model_path='arc_model_non_sim_dynamic_range_quant.tflite')
    interpreter.allocate_tensors()

    output_index = 0
    scale = interpreter.get_output_details()[output_index]['quantization_parameters']['scales']
    zero_point = interpreter.get_output_details()[output_index]['quantization_parameters']['zero_points']

    print(f'scale = {scale}')
    print(f'zero point = {zero_point}')

    # Get input and output details
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()

    print(f'input details = {input_details}')
    print(f'output details = {output_details}')

    # Prepare input data
    input_shape = input_details[0]['shape']


    # Get names and colors
    names = ['pedestrian', 'car', 'motor', 'truck']
    colors = [[random.randint(0, 255) for _ in range(3)] for _ in names]

    # open test set
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


    im = 0
    for line in file:
        img_path = base + line[2:].replace("\n", "")
        label_path = img_path.replace("images", "labels")[:-3] + "txt"

        if not os.path.isfile(img_path) or not os.path.isfile(label_path):
            print("label or image missing")
            print(f"name : {line}")

        # Load image    
        path, img, img0 = LoadImage(img_path, imgsz, 32)
        # img = torch.from_numpy(np.array([img])).to(device)
        # img = img.half() # uint8 to fp16/32
        # img /= 255.0  # 0 - 255 to 0.0 - 1.0
        # frame = img - 128
        # frame = np.array([frame]).astype(np.int8)
        
        frame = img / 255.0
        frame = np.array([frame]).astype(np.float32)
        zero_point = [0]
        scale = [1]

        input_data = frame  # Provide your input data here

        # Set input tensor
        interpreter.set_tensor(input_details[0]['index'], input_data)

        # Run inference

        t1 = time.time()
        interpreter.invoke()
        t2 = time.time()
        # Get output tensor
        output_data = interpreter.get_tensor(output_details[0]['index'])

        pred = myIdetect(output_data, -zero_point[0], scale[0])

        # nms
        if len(pred) > 0:
            pred = np.array([my_nms(pred)])
        t3 = time.time()


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
        p = Path(img_path)
        save_path = str(save_dir / p.name)
        # Process detections
        for i, det in enumerate(pred):
            det = det.astype(np.float)
            det[:, :4] = scale_coords(img.shape[0:2], det[:, :4], img0.shape).round()
            # det[:, 5] *= 30
            for tmp in det:
                pred_test.append(tmp)
            continue
            # Print result on image and save
            for *xyxy, conf, dist, cls in reversed(det):
                label = f'{names[int(cls)]} conf:{conf:.2f} dist:{dist:.2f}m'
                plot_one_box(xyxy, img0, label=label, color=colors[int(cls)], line_thickness=3)
                # dist = dist.cpu()
            # print(frame0.shape)

        all_pred.append(np.array(pred_test))
        # cv2.imwrite(save_path, img0)
        # print(f"image save in : {save_path}")
        print(f'Done. ({(1E3 * (t2 - t1)):.1f}ms) Inference, ({(1E3 * (t3 - t2)):.1f}ms) NMS')
        
        # ignore first 10 image for warmup
        if img_idx > 10:
            total_image += 1
            total_time += t3 - t1
        img_idx += 1


        im += 1
        if im > 500:
            break

    tester = MyTester(all_labels=np.array(all_gt), all_predictions=np.array(all_pred))
    tester.showResult()
    print(f'Done. ({(1E3 * (total_time/total_image)):.1f}ms) average time Inference and NMS')

