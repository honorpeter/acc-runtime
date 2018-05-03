<span style="display: inline-block;">

# File structure
1. [inference_host] host functions run on host-CPU with corresponding accelerator hardware
2. [inference_net_hls] inference functions implemented with software
3. [modelTest] generated test_modelname.cpp functions with different model input
4. [unitTest] contrl/mem/interrupt test functions
5. [utils] accurate time measurement functions
6. [netInput & netOutput] input and output files temp folder
7. Makefile compilation instructions
8. README.md -- this readme.


Currently using agfi:
{testCorework
    "FpgaImageId": "afi-048de40330675dec6", 
    "FpgaImageGlobalId": "agfi-0fbf155fc9c4a86e7"
}
