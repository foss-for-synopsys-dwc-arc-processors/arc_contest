# WOLO-ARC-EM9D
Project for WOLO deploy on Synopsys ARC EM9D

# Model Training and App link
    Model Training : https://github.com/c119cheng/distyolo_v7
    App : https://github.com/coherent17/WOLO-app
    
# Convert model to tensorflow lite for microcontroller INT8
    export onnx file -> convert to tensorflow lite

# Deploy on ARC EM9D
```bash
    $ make
    $ make flash
```