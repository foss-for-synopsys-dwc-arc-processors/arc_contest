import numpy as np
import onnx
import onnxruntime as ort
import torch
import torchvision
import argparse
import cv2

from utils.datasets import LoadStreams, LoadImages
from utils.torch_utils import select_device, load_classifier, time_synchronized, TracedModel
from utils.general import check_img_size, check_requirements, check_imshow, non_max_suppression, apply_classifier

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
    img0 = cv2.cvtColor(img0, cv2.COLOR_BGR2GRAY)
    img0 = cv2.resize(img0, (640, 480))
    img = letterbox(img0, new_shape=img_size, stride=stride)[0]

    # Convert
    # img = img[:, :, ::-1].transpose(2, 0, 1)  # BGR to RGB
    img = [img]
    print(img[0].shape)
    img = np.ascontiguousarray(img)

    return path, img, img0

def torch2onnx():
    # Load model
    device = torch.device('cuda:0') if opt.device != 'cpu' else torch.device('cpu')
    half = device.type != 'cpu'
    ckpt = torch.load(opt.weights[0], map_location=device)
    model = ckpt['ema' if ckpt.get('ema') else 'model'].float().fuse()
    model.eval()
    stride = int(model.stride.max())

    # trace model
    # model = TracedModel(model, device, opt.img_size)
    
    # Load test image
    path, img, img0 = LoadImage(opt.test_input, opt.img_size, stride)
    tmp = img
    img = torch.from_numpy(np.array([img])).to(device)
    img = img.float() # uint8 to fp16/32
    img /= 255.0  # 0 - 255 to 0.0 - 1.0

    # Test in pytorch
    model.model[-1].export = False
    print('pytorch result:', model(img)[0])

    # Config for onnx
    input_names = ['input']
    output_names = ['output']

    # onnx path
    onnx_path = opt.name

    # to onnx
    # opset_version=11 for jetson nano
    torch.onnx.export(model, img, onnx_path, verbose=False, input_names=input_names, 
                      output_names=output_names, opset_version=11)

    # test onnx
    img = np.array([tmp], dtype=np.float32)
    img /= 255.0
    
    model = onnx.load(onnx_path)
    import onnxsim
    model, check = onnxsim.simplify(model)
    onnx.save(model, onnx_path)
    ort_session = ort.InferenceSession(onnx_path)
    outputs = ort_session.run(None, {'input' : img})

    print('onnx result:', outputs[0])

def onnxtest():
    path, img, img0 = LoadImage(opt.test_input, opt.img_size, 32)
    img = np.array([img], dtype=np.float32)
    print(img.shape)
    img /= 255.0
    onnx_path = opt.weights[0]
    model = onnx.load(onnx_path)
    ort_session = ort.InferenceSession(onnx_path)
    pred = ort_session.run(None, {'images' : img})[0]

    # pred = non_max_suppression(torch.from_numpy(pred).to(torch.device('cuda:0')), 0.45, 0.25)
    print('onnx result:', pred)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--weights', nargs='+', type=str, help='model.pt path')
    parser.add_argument('--test-input', type=str, help='input image')
    parser.add_argument('--img-size', nargs='+', type=int, default=[640, 640], help='image size')
    parser.add_argument('--conf-thres', type=float, default=0.25)
    parser.add_argument('--iou-thres', type=float, default=0.45)
    parser.add_argument('--device', default='0')
    parser.add_argument('--project', default='runs/onnx')
    parser.add_argument('--name', default='exp')
    parser.add_argument('--test', action='store_true', help='test onnx file')
    opt = parser.parse_args()
    print(opt)

    if opt.test:
        onnxtest()
    else:
        torch2onnx()