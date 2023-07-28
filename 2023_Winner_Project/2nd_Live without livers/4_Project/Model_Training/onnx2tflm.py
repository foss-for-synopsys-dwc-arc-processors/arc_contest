
# !onnx-tf convert -i [onnx model path] -o [tf model save path]

import tensorflow as tf
import cv2
import numpy as np

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

converter = tf.lite.TFLiteConverter.from_saved_model('tflm-gray')
converter.optimizations = [tf.lite.Optimize.DEFAULT]
converter.target_spec.supported_ops = [
    tf.lite.OpsSet.TFLITE_BUILTINS_INT8
    # tf.lite.OpsSet.SELECT_TF_OPS
]
converter.inference_input_type = tf.uint8
converter.inference_output_type = tf.float16

# converter.experimental_new_quantizer = True
def representative_dataset_gen():
    for _ in range(10):
        input = cv2.imread('./nuscenes/new.jpg')
        input = cv2.cvtColor(input, cv2.COLOR_BGR2GRAY)
        input = letterbox(input, (256, 320), stride=32)[0]
        input = input.reshape(256, 320, 1)
        # img = img[:, :, ::-1].transpose(2, 0, 1)  # BGR to RGB
        # img = np.ascontiguousarray(img)
        yield [input.astype(np.float32)]

converter.representative_dataset = representative_dataset_gen
tflite_model = converter.convert()
with tf.io.gfile.GFile('distyolo-gray-INT8.tflite', 'wb') as f:
    f.write(tflite_model)
print('finish converter')