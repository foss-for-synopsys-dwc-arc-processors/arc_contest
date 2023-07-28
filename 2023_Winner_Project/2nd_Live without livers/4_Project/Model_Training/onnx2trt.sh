#!/bin/bash
# Using for convert onnx to trt on jetson nano

# Reference :
# https://zhuanlan.zhihu.com/p/467401558
# https://developer.nvidia.com/zh-cn/blog/tensorrt-trtexec-cn/
# https://github.com/NVIDIA/TensorRT/tree/main/samples/trtexec
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin
export PATH

# read -p "Please input your onnx file:" onnx
# read -p "Please input output engine name:" engine
echo "\nConvert $1 to $2"

/usr/src/tensorrt/bin/trtexec --onnx=$1 --saveEngine=$2 --best