// Amazon FPGA Hardware Development Kit
//
// Copyright 2016 Amazon.com, Inc. or its affiliates. All Rights Reserved.
//
// Licensed under the Amazon Software License (the "License"). You may not use
// this file except in compliance with the License. A copy of the License is
// located at
//
//    http://aws.amazon.com/asl/
//
// or in the "license" file accompanying this file. This file is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, express or
// implied. See the License for the specific language governing permissions and
// limitations under the License.


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>

//C++ headers
#include <cstdint>
#include <iostream>
//#include <cstring>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include <fpga_pci.h>
#include <fpga_mgmt.h>
#include <utils/lcd.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../inference_net_hls/stb_image.h"
#include "../inference_net_hls/weight_bias_one_dim.h"
#include "../inference_net_hls/config.h"
#include "../inference_net_hls/softmax_one_dim.h"
#include "../inference_net_hls/predict_one_dim.h"

#include "../inference_host/inference_func.h"
#include "../inference_host/acc_mem_config.h"
#include "../inference_host/construct_net.h"
#include "../inference_host/construct_layer.h"
#include "../utils/cl_tsc.h"

/* use the stdout logger */
const struct logger *logger = &logger_stdout;

using namespace std;

int main(int argc, char **argv) {
    int rc;
    int slot_id;

    /* initialize the fpga_pci library so we could have access to FPGA PCIe from this applications */
    rc = fpga_pci_init();
    fail_on(rc, out, "Unable to initialize the fpga_pci library");

    /* This demo works with single FPGA slot, we pick slot #0 as it works for both f1.2xl and f1.16xl */
    slot_id = 0;

    rc = check_afi_ready(slot_id);
    fail_on(rc, out, "AFI not ready");
    
    /* Accessing the CL registers via AppPF BAR0, which maps to sh_cl_ocl_ AXI-Lite bus between AWS FPGA Shell and the CL*/
    printf("\n");

    //printf("===== AXI CDMA Example =====\n");   
    rc = peek_poke_example(slot_id, FPGA_APP_PF, APP_PF_BAR1);
    fail_on(rc, out, "peek-poke example failed");
  
    return rc; 
   
out:
    return 1;
}

/*
 * An example to attach to an arbitrary slot, pf, and bar with register access.
 */
int peek_poke_example(int slot_id, int pf_id, int bar_id) {
    int rc;
    int rc_0;
    int rc_4;
    int rc_sda;

    int loop_var;
    int loop_var_1;
    int loop_var_2;
    int layer_count;

    data_type in_data[32*32*32];
    data_type out_data[32*32*32];
    data_type in_data_1[32*32];

    //data_buf matrix
    float *in_data_buf;

    data_type  fc_3_out[10];
    data_type_w conv_weight[6*5*5+2400+4000];
    data_type_w conv_bias[6+16+10];
    float conv_1_weight2D[150];
    float conv_1_bias2D[6];
    float conv_2_weight2D[2400];
    float conv_2_bias2D[16];
    float fc_1_weight2D[4000];
    float fc_1_bias2D[10];
    float conv_weight_tmp[8][32][1024];

    int loop_param[3][18];
    int conv_param[3][16];

    int acc_param_conv[3][16];
    int acc_param_pool[3][16];

    // conv param order = { n, k, m, Rin, Cin, Rout, Cout, S, P, act }
    int conv_param_1[16] = {1, 5, 6, 28, 28, 28, 28, 1, 2, 1, 0, 0, 0, 0, 1, 1};
    // pool param order = { C_in, R_in, N, K, C_out, R_out, stride, pad, act }
    int pool_param_1[16] = {28, 28, 6, 2, 14, 14, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0};
    int conv_param_2[16] = {6, 5, 16, 14, 14, 10, 10, 1, 0, 1, 0, 0, 0, 0, 1, 1};
    int pool_param_2[16] = {10, 10, 16, 2, 5, 5, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0};
    int conv_param_3[16] = {16, 5, 10, 5, 5, 1, 1, 5, 0, 1, 0, 0, 0, 0, 1, 1};
    int pool_param_3[16] = {10, 10, 10, 2, 1, 1, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0};

    int acc_param_test[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int acc_param_conv_1[16] = {1/*S*/, 0/*n*/, 0/*r*/, 0/*c*/, 5/*K*/, 0/*r_offset*/, 0/*c_offset*/, 1/*N*/, 0/*P*/, 0/*w_r_offset*/, 0/*w_c_offset*/, 0, 0/*in_buf_flag*/, 0/*out_buf_flag*/, 0, 0};
    int acc_param_conv_2[16] = {1/*S*/, 0/*n*/, 0/*r*/, 0/*c*/, 5/*K*/, 0/*r_offset*/, 0/*c_offset*/, 6/*N*/, 0/*P*/, 0/*w_r_offset*/, 5/*w_c_offset*/, 0, 0/*in_buf_flag*/, 0/*out_buf_flag*/, 0, 0};
    int acc_param_conv_3[16] = {5/*S*/, 0/*n*/, 0/*r*/, 0/*c*/, 5/*K*/, 0/*r_offset*/, 0/*c_offset*/, 16/*N*/, 0/*P*/, 0/*w_r_offset*/, 10/*w_c_offset*/, 0, 0/*in_buf_flag*/, 0/*out_buf_flag*/, 0, 0};
                             
    int acc_param_pool_1[16] = {2/*S*/, 0/*n*/, 0/*r*/, 0/*c*/, 2/*K*/, 28/*in_size*/, 28/*in_size*/, 0/*P*/, 0, 0, 0, 0, 0, 0, 0, 0};
    int acc_param_pool_2[16] = {2/*S*/, 0/*n*/, 0/*r*/, 0/*c*/, 2/*K*/, 10/*in_size*/, 10/*in_size*/, 0/*P*/, 0, 0, 0, 0, 0, 0, 0, 0};
    int acc_param_pool_3[16] = {1/*S*/, 0/*n*/, 0/*r*/, 0/*c*/, 1/*K*/, 1/*in_size*/, 1/*in_size*/, 0/*P*/, 0, 0, 0, 0, 0, 0, 0, 0};

    int w;
    int h;
    int channels;
    int size;
    const unsigned char * data ;
    const unsigned char * image_orig ;
    int in_number_conv = 0;
    int in_number_fc = 0;
    int in_number_pooling = 0;
    int conv_weight_num =0 ;
    int conv_bias_num = 0;
    int K_max_last_r = 0;

    string image_dir = "./netInput/3.bmp";
    const char* weight_src = "./netInput/net_weights.txt";
    std::ofstream indata;
    std::ofstream outdata;
    std::ofstream weightdata;
    float in_data_3D_padding[1][32][32] = { 0 };
    int in_data_size = 0;
    int w_r_offset = 0;
    int w_c_offset = 0;
    std::ofstream w_buf_t;

    //time mreasurement variable define
    uint64_t start_time,end_time;
    uint64_t start_net,end_net;
    struct timeval start,end;
    unsigned long diff;
    XInference_net InstancePtr;
    InstancePtr.ctrl_bus_baseaddress = XINFERENCE_IP_CRTL_BUS_ADDR_1;
    InstancePtr.IsReady = 0x01;
    uint32_t ip_status;

    int fd, rc1;
    char device_file_name[256];
    fd = -1;

    /* attach to the fpga, with a pci_bar_handle out param
     * To attach to multiple slots or BARs, call this function multiple times,
     * saving the pci_bar_handle to specify which address space to interact with in
     * other API calls.
     * This function accepts the slot_id, physical function, and bar number
     */
    rc = fpga_pci_attach(slot_id, pf_id, bar_id, 0, &pci_bar_handle);
    fail_on(rc, out, "Unable to attach to the AFI on slot id %d", slot_id);

    rc_0 = fpga_pci_attach(slot_id, pf_id, 0, 0, &pci_bar_handle_0);
    fail_on(rc_0, out, "Unable to attach to the AFI on slot id %d", slot_id);

    rc_4 = fpga_pci_attach(slot_id, pf_id, 4, 0, &pci_bar_handle_4);
    fail_on(rc_4, out, "Unable to attach to the AFI on slot id %d", slot_id);

    rc_sda = fpga_pci_attach(slot_id, FPGA_MGMT_PF, MGMT_PF_BAR4, 0, &pci_bar_handle_sda);
    fail_on(rc_sda, out, "Unable to attach to the AFI on slot id %d", slot_id);

    rc1 = log_init("test_dram_dma");
    //fail_on(rc1, out, "Unable to initialize the log.");
    rc1 = log_attach(logger, NULL, 0);
    fail_on(rc1, out, "%s", "Unable to attach to the log.");

    /* initialize the fpga_plat library */
    rc1 = fpga_mgmt_init();
    fail_on(rc1, out, "Unable to initialize the fpga_mgmt library");
    
    rc1 = sprintf(device_file_name, "/dev/edma%i_queue_0", slot_id);
    fail_on((rc1 = (rc1 < 0)? 1:0), out, "Unable to format device file name.");
    printf("device_file_name=%s\n\n", device_file_name);

    /* make sure the AFI is loaded and ready */
    rc1 = check_slot_config(slot_id);
    fail_on(rc1, out, "slot config is not correct");

    fd = open(device_file_name, O_RDWR);
    if (fd<0) {
        printf("Cannot open device file %s.\nMaybe the EDMA "
               "driver isn't installed, isn't modified to attach to the PCI ID of "
               "your CL, or you're using a device file that doesn't exist?\n"
               "See the edma_install manual at <aws-fpga>/sdk/linux_kernel_drivers/edma/edma_install.md\n"
               "Remember that rescanning your FPGA can change the device file.\n"
               "To remove and re-add your edma driver and reset the device file mappings, run\n"
               "`sudo rmmod edma-drv && sudo insmod <aws-fpga>/sdk/linux_kernel_drivers/edma/edma-drv.ko`\n",
               device_file_name);
        fail_on((rc1 = (fd < 0)? 1:0), out, "unable to open DMA queue. ");
    }

//--------------------------input image data initialization----------------//
    data = loadfile(image_dir, size);
    image_orig = stbi_load_from_memory(data, size, &w, &h, &channels, 1);
    for (loop_var = 0; loop_var < 28*28; loop_var++) {
        in_data[loop_var] = (data_type)image_orig[loop_var];
    }
    //add padding for input
    for (loop_var = 0; loop_var < 1; loop_var++) {
        for (loop_var_1 = 0; loop_var_1 < 32; loop_var_1++) {
            for (loop_var_2 = 0; loop_var_2 < 32; loop_var_2++) {
                in_data_3D_padding[loop_var][loop_var_1][loop_var_2] = 0;
            }
        }
    }
    for (loop_var = 0; loop_var < 1; loop_var++) {
        for (loop_var_1 = 0; loop_var_1 < 28; loop_var_1++) {
            for (loop_var_2 = 0; loop_var_2 < 28; loop_var_2++) {
                in_data_3D_padding[loop_var][loop_var_1+2][loop_var_2+2] = (data_type)in_data[loop_var*28*28+loop_var_1*28+loop_var_2];
            }
        }
    }
    for (loop_var = 0; loop_var < 1; loop_var++) {
        for (loop_var_1 = 0; loop_var_1 < 32; loop_var_1++) {
            for (loop_var_2 = 0; loop_var_2 < 32; loop_var_2++) {
                in_data[in_data_size] = (data_type)in_data_3D_padding[loop_var][loop_var_1][loop_var_2];
                in_data_size++;
            }
        }
    }

//----------------------input weight data initialization ------------------//
    // Prepare weights and bias for conv layer 1
    memset(conv_1_weight2D, 0, 150 * sizeof(float));
    load_weight_conv(
        weight_src, 
        conv_1_weight2D,
        weight_bias_record,
        nn_channel_size_conv, 
        nn_in_number_conv,
        nn_out_number_conv,
        in_number_conv);
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 5; j++) {
            for (int k = 0; k < 5; k++) {
                conv_weight[conv_weight_num] = (data_type_w)conv_1_weight2D[i*5*5+j*5+k];
                conv_weight_num++;
            }
        }
    }
    memset(conv_1_bias2D, 0, 6 * sizeof(float));
    load_bias_conv(
        weight_src, 
        conv_1_bias2D,
        weight_bias_record,
        nn_channel_size_conv, 
        nn_in_number_conv,
        nn_out_number_conv,
        in_number_conv);
    for (int i = 0; i < 6; i++) {
        conv_bias[conv_bias_num] = (data_type_w)conv_1_bias2D[i];
        conv_bias_num++;
    }
    in_number_conv++;

    // Prepare weights and bias for conv layer 2
    memset(conv_2_weight2D, 0, 2400 * sizeof(float));
    load_weight_conv(
        weight_src, 
        conv_2_weight2D,
        weight_bias_record,
        nn_channel_size_conv, 
        nn_in_number_conv,
        nn_out_number_conv,
        in_number_conv);
    for (int i = 0; i < 2400; i++) {
        conv_weight[conv_weight_num] = (data_type_w)conv_2_weight2D[i];
        conv_weight_num++;
    }
    memset(conv_2_bias2D, 0, 16 * sizeof(float));
    load_bias_conv(
        weight_src, 
        conv_2_bias2D,
        weight_bias_record,
        nn_channel_size_conv, 
        nn_in_number_conv,
        nn_out_number_conv,
        in_number_conv);
    for (int i = 0; i < 16; i++) {
        conv_bias[conv_bias_num] = (data_type_w)conv_2_bias2D[i];
        conv_bias_num++;
    }
    in_number_conv++;

    // Prepare weights and bias for fc layer 1
    memset(fc_1_weight2D, 0, 4000 * sizeof(float));
    load_weight_fc(
        weight_src, 
        fc_1_weight2D,
        weight_bias_record,
        nn_channel_size_fc, 
        nn_in_number_fc,
        nn_out_number_fc,
        in_number_fc);
    for (int i = 0; i < 4000; i++) {
        conv_weight[conv_weight_num] = (data_type_w)fc_1_weight2D[i];
        conv_weight_num++;
    }
    memset(fc_1_bias2D, 0, 10 * sizeof(float));
    load_bias_fc(
        weight_src, 
        fc_1_bias2D,
        weight_bias_record,
        nn_channel_size_fc, 
        nn_in_number_fc,
        nn_out_number_fc,
        in_number_fc);
    for (int i = 0; i < 10; i++) {
        conv_bias[conv_bias_num] = (data_type_w)fc_1_bias2D[i];
        conv_bias_num++;
    }
    in_number_fc++;

//---------------------conv weight reorder ------------------------------------//
    //conv_1_w_load
    w_buf_t_load(conv_weight_tmp, conv_1_weight2D, 0, conv_param_1[1], conv_param_1[0], conv_param_1[2], w_r_offset, w_c_offset, K_max_last_r);

    //conv_2_w_load
    w_buf_t_load(conv_weight_tmp, conv_2_weight2D, 0, conv_param_2[1], conv_param_2[0], conv_param_2[2], w_r_offset, w_c_offset, K_max_last_r);
    
    //fc_w_load
    w_buf_t_load(conv_weight_tmp, fc_1_weight2D, 0, conv_param_3[1], conv_param_3[0], conv_param_3[2], w_r_offset, w_c_offset, K_max_last_r);

    w_buf_t.open("netOutput/weight.txt", ios::app);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 32; j++) {
            w_buf_t <<"w_buf_data: "<<i<<"_"<<j<< endl;
            for (int k = 0; k < 32; k++) {
                for (int l = 0; l < 32; l++) {
                    w_buf_t << conv_weight_tmp[i][j][k*32+l] << " ";
                }
                w_buf_t << endl;
            }
            w_buf_t << endl;
        }
        w_buf_t << endl;
    }
    w_buf_t.close();

    /*for (int i = 0; i < 1; i++ ) {
        for (int j = 0; j < 6; j++) {
            for (int k = 0; k < 5; k++) {
                for (int l = 0; l < 5; l++) {
                    conv_weight_tmp[i][j][k*32 + l] = conv_1_weight2D[j*25 + k*5 +l];
                }
            }
        }
    }
    Fill_Bram(pci_bar_handle_4, CONV_W_BRAM_PCIS_0_0, conv_weight_tmp[0][0], 32*5);
    Fill_Bram(pci_bar_handle_4, CONV_W_BRAM_PCIS_0_1, conv_weight_tmp[0][1], 32*5);
    Fill_Bram(pci_bar_handle_4, CONV_W_BRAM_PCIS_0_2, conv_weight_tmp[0][2], 32*5);
    Fill_Bram(pci_bar_handle_4, CONV_W_BRAM_PCIS_0_3, conv_weight_tmp[0][3], 32*5);
    Fill_Bram(pci_bar_handle_4, CONV_W_BRAM_PCIS_0_4, conv_weight_tmp[0][4], 32*5);
    Fill_Bram(pci_bar_handle_4, CONV_W_BRAM_PCIS_0_5, conv_weight_tmp[0][5], 32*5);
    */
    //in_data
    //Fill_Bram(pci_bar_handle_4, DDR_A_ADDR_1, in_data, 32*32);
    //Fill_Bram(pci_bar_handle_4, DDR_B_ADDR, conv_bias, 6+16+10);
    
    //----------------------weight data buffer load------------------------------//
    //load all weight
    for(loop_var_1 = 0; loop_var_1 < 16; loop_var_1++){
        Fill_Bram(pci_bar_handle_4, CONV_W_BRAM_PCIS_0_0+loop_var_1*0x00001000, conv_weight_tmp[0][loop_var_1], 32*5);
    }
    for(loop_var = 0; loop_var < 7; loop_var++){
        for(loop_var_1 = 0; loop_var_1 < 16; loop_var_1++){
            //set_cdma(pci_bar_handle,0x01100000+loop_var*0x00100000+loop_var_1*0x00001000,0x0000000E,0x06020000+loop_var*0x00020000+loop_var_1*0x00001000,0x00000000,0x00000280);
            Fill_Bram(pci_bar_handle_4, 0x06020000+loop_var*0x00020000+loop_var_1*0x00001000, conv_weight_tmp[loop_var+1][loop_var_1], 32*5);
        }  
    } 
    
    //load input
    //set_cdma(pci_bar_handle,0x02000000,0x0000000C,0x02000000,0x00000000,0x00001000);

    //conv & pool param matrix
    for (loop_var = 0; loop_var < 16; loop_var ++) {
        conv_param[0][loop_var] = conv_param_1[loop_var];
        conv_param[1][loop_var] = conv_param_2[loop_var];
        conv_param[2][loop_var] = conv_param_3[loop_var];
    }
    for (loop_var = 0; loop_var < 16; loop_var ++) {
        acc_param_conv[0][loop_var] = acc_param_conv_1[loop_var];
        acc_param_conv[1][loop_var] = acc_param_conv_2[loop_var];
        acc_param_conv[2][loop_var] = acc_param_conv_3[loop_var];
        acc_param_pool[0][loop_var] = acc_param_pool_1[loop_var];
        acc_param_pool[1][loop_var] = acc_param_pool_2[loop_var];
        acc_param_pool[2][loop_var] = acc_param_pool_3[loop_var];
    }
    layer_count = 3;

    //loop_param matrix
    for (loop_var = 0; loop_var < 3; loop_var ++) {
        //iter_n
        loop_param[loop_var][0] = static_cast<int>(ceil(static_cast<float>(conv_param[loop_var][0]) / Tn));
        //iter_m
        loop_param[loop_var][1] = static_cast<int>(ceil(static_cast<float>(conv_param[loop_var][2]) / Tm));
        //iter_r_in : 
        loop_param[loop_var][2] = static_cast<int>(ceil(static_cast<float>(IBUF_t < (conv_param[loop_var][3] + 2 * conv_param[loop_var][8]) ? IBUF_t : (conv_param[loop_var][3] + 2 * conv_param[loop_var][8])) / Tr));
        //iter_c_in
        loop_param[loop_var][3] = static_cast<int>(ceil(static_cast<float>(IBUF_t < (conv_param[loop_var][4] + 2 * conv_param[loop_var][8]) ? IBUF_t : (conv_param[loop_var][4] + 2 * conv_param[loop_var][8])) / Tc));
        //iter_r_in_buf : iterations of IBUF_t on in_data row dimension
        loop_param[loop_var][4] = static_cast<int>(ceil(static_cast<float>((conv_param[loop_var][3] + 2 * conv_param[loop_var][8])) / IBUF_t));
        //iter_c_in_buf : iterations of IBUF_t on in_data col dimension
        loop_param[loop_var][5] = static_cast<int>(ceil(static_cast<float>((conv_param[loop_var][4] + 2 * conv_param[loop_var][8])) / IBUF_t));
        //iter_r_out_buf : iterations of OBUF_t on out_data row dimension
        loop_param[loop_var][6] = static_cast<int>(ceil(static_cast<float>(conv_param[loop_var][5]) / OBUF_t));
        //iter_c_out_buf : iterations of OBUF_t on out_data col dimension
        loop_param[loop_var][7] = static_cast<int>(ceil(static_cast<float>(conv_param[loop_var][6]) / OBUF_t));
        
        //offset_n : in_data n dimension at each iteration of N
        loop_param[loop_var][8] = Tn < conv_param[loop_var][0] ? Tn : conv_param[loop_var][0];
        //offset_m : out_data m dimension at each iteration of M
        loop_param[loop_var][9] = Tm < conv_param[loop_var][2] ? Tm : conv_param[loop_var][2];
        
    }

    //reorder in_data
    in_data_buf = (float *) malloc (Tn * IBUF_t * IBUF_t * loop_param[2][0] * loop_param[2][4] * loop_param[2][5] * sizeof(float));
    in_data_reorder(loop_param[0], acc_param_conv[0], in_data_buf, in_data);
    //check in_data_buf
    for(loop_var = 0; loop_var < 1; loop_var++){
        for(loop_var_1 = 0; loop_var_1 < 32; loop_var_1++){
            for(loop_var_2 = 0; loop_var_2 < 32; loop_var_2++){
                cout << in_data_buf[loop_var*32*32+loop_var_1*32+loop_var_2] << "  ";
            }
            cout << endl;  
        }
        cout << endl;   
    }
    cout << endl;
    
    start_net = cycles_to_microseconds(ticks());
//----------------------inference net ip status check -----------------------//    
    inference_net(rc1, fd, &InstancePtr, layer_count, loop_param, conv_param, 
        acc_param_conv, acc_param_pool, in_data_buf, in_data, out_data, conv_bias);
    
    // cout << "out_data: " << endl; 
    // for(loop_var = 0; loop_var < 10; loop_var++){
    //     for(loop_var_1 = 0; loop_var_1 < 1; loop_var_1++){
    //         for(loop_var_2 = 0; loop_var_2 < 32; loop_var_2++){
    //             cout << out_data[loop_var*32*32+loop_var_1*32+loop_var_2] << "  ";
    //         }
    //         cout << endl;  
    //     }
    //     cout << endl;   
    // }
    // cout << endl;

    // Read_param(pci_bar_handle_4, POOL_CORE_PARAM, acc_param_test, 16);
    // cout << "POOL_CORE_PARAM: " << endl; 
    // for(loop_var = 0; loop_var < 16; loop_var++){
    //     cout << acc_param_test[loop_var] << "  ";  
    // }
    // cout << endl; 
    // Read_param(pci_bar_handle_4, CONV_CORE_PARAM, acc_param_test, 16);
    // cout << "CONV_CORE_PARAM: " << endl; 
    // for(loop_var = 0; loop_var < 16; loop_var++){
    //     cout << acc_param_test[loop_var] << "  ";  
    // }
    // cout << endl;    
    // cout << endl;

    for (loop_var = 0; loop_var < 10; loop_var ++) {
        fc_3_out[loop_var] = out_data[loop_var * IBUF_t * IBUF_t];
    }
    
    cout << "Classified result is: " << endl;
    for (loop_var = 0; loop_var < 10; loop_var++ ) {
        cout << fc_3_out[loop_var] << "  ";
    }
    cout << endl;
    cout << endl;

    softmax(fc_3_out, 10);
    predict(fc_3_out, 10);

    end_net = cycles_to_microseconds(ticks());
    cout << "Lenet processing time = " << end_net - start_net << "  us" << endl;

    /*if (in_data_buf != NULL) {
        free(in_data_buf);
    }*/
//------------------------------------------------------------------------------------------

out:
    /* clean up */
    if (pci_bar_handle >= 0) {
        rc = fpga_pci_detach(pci_bar_handle);
        if (rc) {
            printf("Failure while detaching from the fpga.\n");
        }
    }

    if (pci_bar_handle_0 >= 0) {
        rc_0 = fpga_pci_detach(pci_bar_handle_0);
        if (rc_0) {
            printf("Failure while detaching from the fpga.\n");
        }
    }

    if (pci_bar_handle_4 >= 0) {
        rc_4 = fpga_pci_detach(pci_bar_handle_4);
        if (rc_4) {
            printf("Failure while detaching from the fpga.\n");
        }
    }


    if (pci_bar_handle_sda >= 0) {
        rc_sda = fpga_pci_detach(pci_bar_handle_sda);
        if (rc_sda) {
            printf("Failure while detaching from the fpga.\n");
        }
    }

    /* if there is an error code, exit with status 1 */
    return (rc != 0 ? 1 : 0);
}
