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
#include "../inference_net_hls/acc_dim_config.h"
#include "../inference_host/inference_func.h"
#include "../inference_host/acc_mem_config.h"
#include "../inference_net_hls/max_pool_acc_innerpp.h"
#include "../inference_net_hls/acc_instance.h"
#include "../inference_net_hls/softmax_one_dim.h"
#include "../inference_net_hls/predict_one_dim.h"
#include "../utils/cl_tsc.h"

using namespace std;

/* use the stdout logger for printing debug information  */
const struct logger *logger = &logger_stdout;

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

    printf("===== AXI CDMA Example =====\n");   
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
    uint32_t value;

    int loop_var;
    int loop_var_1;
    int loop_var_2;

    data_type in_data[3*36*36];
    data_type  out_temp_full[8192];
    data_type  out_temp_1[1024];
    data_type  out_temp_2[1024];
    data_type  out_temp_3[1024];
    data_type  out_temp_4[1024];
    data_type  out_temp_5[1024];
    data_type  out_temp_6[1024];
    data_type  out_temp_7[1024];
    data_type  out_temp_8[1024];
    data_type  out_temp_9[1024];
    data_type  out_temp_10[1024];
    data_type  fc_3_out[10];
    data_type_w conv_weight[2400+25600+51200+10240];
    data_type_w conv_weight_1[1]={1};
    data_type_w conv_weight_2[2400+25600+51200+10240];
    data_type_w conv_bias[32+32+64+10];
    data_type_w conv_bias_1[32+32+64+10];
    float conv_1_weight2D[2400];
    float conv_1_bias2D[32];
    float conv_2_weight2D[25600];
    float conv_2_bias2D[32];
    float conv_3_weight2D[51200];
    float conv_3_bias2D[64];
    float fc_1_weight2D[10240];
    float fc_1_bias2D[10];
    float conv_weight_tmp[8][32][1024];
    
    int ctrl_param_1[2] = {1, 0};
    int ctrl_param_2[2] = {0, 1};

    // param order = {n, k, m, Rin, Cin, Rout, Cout, S, P, act}
    int conv_param_1[16] = {3, 5, 8, 32, 32, 32, 32, 1, 2, 1, 0, 0, 0, 0, 1, 1};
    int conv_param_2[16] = {32, 5, 32, 16, 16, 12, 12, 1, 0, 1, 0, 0, 0, 0, 1, 1};
    int conv_param_3[16] = {32, 5, 64, 6, 6, 2, 2, 1, 0, 1, 0, 0, 0, 0, 1, 1};
    int conv_param_4[16] = {64, 1, 10, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1};
    int pool_param_1[9] = {32, 32, 32, 2, 16, 16, 2, 0, 0};
    int pool_param_2[9] = {12, 12, 32, 2, 6, 6, 2, 0, 0};
    int pool_param_3[9] = {2, 2, 64, 2, 1, 1, 2, 0, 0};

    int acc_param_conv_1[16] = {1/*S*/, 0/*n*/, 0/*r*/, 0/*c*/, 5/*K*/, 0, 0, 3/*N*/, 1, 0, 0, 0, 0, 0, 0, 0};
    int acc_param_test[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int acc_param_conv_2[16] = {1/*S*/, 0/*n*/, 0/*r*/, 0/*c*/, 5/*K*/, 0, 0, 32/*N*/, 1, 0, 5, 6, 0, 0, 0, 0};
    int acc_param_conv_3[16] = {1/*S*/, 0/*n*/, 0/*r*/, 0/*c*/, 5/*K*/, 0, 0, 32/*N*/, 1, 0, 10, 22, 0, 0, 0, 0};
    int acc_param_conv_4[16] = {1/*S*/, 0/*n*/, 0/*r*/, 0/*c*/, 1/*K*/, 0, 0, 64/*N*/, 1, 0, 10, 22, 0, 0, 0, 0};
                             //C_in,R_in,N,K,C_out,R_out,stride,pad,act
    int acc_param_pool_1[16] = {2/*S*/, 0/*n*/, 0/*r*/, 0/*c*/, 2/*K*/, 32/*in_size*/, 32/*in_size*/, 0/*P*/, 16, 0, 0, 0, 0, 0, 0, 0};
    int acc_param_pool_2[16] = {2/*S*/, 0/*n*/, 0/*r*/, 0/*c*/, 2/*K*/, 12/*in_size*/, 12/*in_size*/, 0/*P*/, 16, 0, 0, 0, 0, 0, 0, 0};
    int acc_param_pool_3[16] = {2/*S*/, 0/*n*/, 0/*r*/, 0/*c*/, 2/*K*/, 2/*in_size*/, 2/*in_size*/, 0/*P*/, 0, 0, 0, 0, 0, 0, 0, 0};
    int acc_param_pool_4[16] = {0/*S*/, 0/*n*/, 0/*r*/, 0/*c*/, 0/*K*/, 1/*in_size*/, 1/*in_size*/, 0/*P*/, 0, 0, 0, 0, 0, 0, 0, 0};

    int w;
    int h;
    int channels;
    int size;
    const unsigned char * data ;
    const unsigned char * image_orig ;
    int in_number_conv = 0;
    int in_number_fc = 0;
    int in_number_pooling = 0;
    int conv_weight_num=0;
    int conv_bias_num=0;

    string image_dir = "./netInput/50000.png";
    const char* weight_src = "./netInput/net_weights_cifar.txt";
    ifstream ifs1("./netInput/net_mean.txt");
    float f;
    float channel_mean[3] = { 0 };
    float in_data_3D_channel_swap[3][32][32] = { 0 };
    float in_data_3D[3][32][32] = { 0 };
    string str1;
    string y1 = "[";
    string y2 = "]";
    int index = 0;
    int in_data_size=0;
    std::ofstream indata;
    std::ofstream outdata;
    std::ofstream weightdata;
    std::ofstream test_output;
    float in_data_3D_padding[3][32][32] = { 0 };
    int w_r_offset = 0;
    int w_c_offset = 0;
    std::ofstream w_buf_t;

    //int i,j,k;
    int count = 0;

    //time mreasurement variable define
    uint64_t start_time,end_time;
    struct timeval start,end;
    unsigned long diff;
    XInference_net InstancePtr;
    InstancePtr.ctrl_bus_baseaddress = XINFERENCE_IP_CRTL_BUS_ADDR_1;
    InstancePtr.IsReady = 0x01;
    uint32_t ip_status;
    /* pci_bar_handle_t is a handler for an address space exposed by one PCI BAR on one of the PCI PFs of the FPGA */

    pci_bar_handle_t pci_bar_handle = PCI_BAR_HANDLE_INIT;
    pci_bar_handle_t pci_bar_handle_0 = PCI_BAR_HANDLE_INIT;
    pci_bar_handle_t pci_bar_handle_4 = PCI_BAR_HANDLE_INIT;

    pci_bar_handle_t pci_bar_handle_sda = PCI_BAR_HANDLE_INIT;

    cout << "test point 1" << endl;

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

//--------------------------input image data initialization----------------//
    if (!ifs1) {
        cout << "mean data file not found !" << endl;
        getchar();
    }
    while (ifs1 >> str1) {
        int p1 = str1.find(y1, 0);
        if (p1 >= 0) {
            str1.erase(p1, y1.length());
        }
        int p2 = str1.find(y2, 0);
        if (p2 >= 0) {
            str1.erase(p2, y2.length());
        }
        f = atof(str1.c_str());
        channel_mean[index] = f;
        index++;
    }
    ifs1.close();
    data = loadfile(image_dir, size);
    image_orig = stbi_load_from_memory(data, size, &w, &h, &channels, 3);
    for (loop_var = 0; loop_var < 3; loop_var++) {
        for (loop_var_1 = loop_var; loop_var_1 < w*h*3; loop_var_1 += 3) {
            in_data_3D_channel_swap[2 - loop_var][loop_var_1 / (w * 3)][(loop_var_1 % (w * 3) - loop_var) / 3] = (float)image_orig[loop_var_1]; //range:0--255
        }
    }
    for (loop_var = 0; loop_var < 3; loop_var++) {
        for (loop_var_1 = 0; loop_var_1 < 32; loop_var_1++) {
            for (loop_var_2 = 0; loop_var_2 < 32; loop_var_2++) {
                in_data_3D_channel_swap[loop_var][loop_var_1][loop_var_2] /= 255;// range:0--1
            }
        }
    }
    for (loop_var = 0; loop_var < 3; loop_var++) {
        for (loop_var_1 = 0; loop_var_1 < 32; loop_var_1++) {
            for (loop_var_2 = 0; loop_var_2 < 32; loop_var_2++) {
                in_data_3D[loop_var][loop_var_1][loop_var_2] = in_data_3D_channel_swap[loop_var][loop_var_1][loop_var_2] * 255 - channel_mean[loop_var];
            }
        }
    }
     //add padding for input
    for (loop_var = 0; loop_var < 3; loop_var++) {
        for (loop_var_1 = 0; loop_var_1 < 36; loop_var_1++) {
            for (loop_var_2 = 0; loop_var_2 < 36; loop_var_2++) {
                in_data_3D_padding[loop_var][loop_var_1][loop_var_2] = 0;
            }
        }
    }
    for (loop_var = 0; loop_var < 3; loop_var++) {
        for (loop_var_1 = 0; loop_var_1 < 32; loop_var_1++) {
            for (loop_var_2 = 0; loop_var_2 < 32; loop_var_2++) {
                in_data_3D_padding[loop_var][loop_var_1+2][loop_var_2+2] = (data_type)in_data_3D[loop_var][loop_var_1][loop_var_2];
            }
        }
    }
    for (loop_var = 0; loop_var < 3; loop_var++) {
        for (loop_var_1 = 0; loop_var_1 < 36; loop_var_1++) {
            for (loop_var_2 = 0; loop_var_2 < 36; loop_var_2++) {
                in_data[in_data_size] = (data_type)in_data_3D_padding[loop_var][loop_var_1][loop_var_2];
                in_data_size++;
            }
        }
    }
    indata.open("./netOutput/in_data.txt", ios::app);
    for (loop_var = 0; loop_var < conv_param_1[0]; loop_var++) {
        for (loop_var_1 = 0; loop_var_1 < 36; loop_var_1++) {
            for (loop_var_2 = 0; loop_var_2 < 36; loop_var_2++) {
                indata << in_data[loop_var*36*36 + loop_var_1*36 + loop_var_2] << " ";
            }
            indata << endl;
        }
        indata << endl;
    }
    indata << endl;
    indata.close();
    cout << endl;

//----------------------input weight data initialization ------------------//
    // Prepare weights and bias for conv layer 1
    memset(conv_1_weight2D, 0, 2400 * sizeof(float));
    load_weight_conv(
        weight_src, 
        conv_1_weight2D,
        weight_bias_record,
        nn_channel_size_conv, 
        nn_in_number_conv,
        nn_out_number_conv,
        in_number_conv);
    //cout << "Loading conv weight 1 to memory space, starting at: " <<conv_weight_num << '\n';
    for (int i = 0; i < 2400; i++) {
        conv_weight[conv_weight_num] = (data_type_w)conv_1_weight2D[i];
        conv_weight_num++;
   }
    memset(conv_1_bias2D, 0, 32 * sizeof(float));
    load_bias_conv(
        weight_src, 
        conv_1_bias2D,
        weight_bias_record,
        nn_channel_size_conv, 
        nn_in_number_conv,
        nn_out_number_conv,
        in_number_conv);
    //cout << "Loading conv bias 1 to memory space, starting at: " <<conv_bias_num << '\n';
    for (int i = 0; i < 32; i++) {
        conv_bias[conv_bias_num] = (data_type_w)conv_1_bias2D[i];
        conv_bias_num++;
    }
    in_number_conv++;

    // Prepare weights and bias for conv layer 2
    memset(conv_2_weight2D, 0, 25600 * sizeof(float));
    load_weight_conv(
        weight_src, 
        conv_2_weight2D,
        weight_bias_record,
        nn_channel_size_conv, 
        nn_in_number_conv,
        nn_out_number_conv,
        in_number_conv);
    //cout << "Loading conv weight 2 to memory space, starting at: " <<conv_weight_num << '\n';
    for (int i = 0; i < 25600; i++) {
        conv_weight[conv_weight_num] = (data_type_w)conv_2_weight2D[i];
        conv_weight_num++;
    }
    memset(conv_2_bias2D, 0, 32 * sizeof(float));
    load_bias_conv(
        weight_src, 
        conv_2_bias2D,
        weight_bias_record,
        nn_channel_size_conv, 
        nn_in_number_conv,
        nn_out_number_conv,
        in_number_conv);
    //cout << "Loading conv bias 2 to memory space, starting at: " <<conv_bias_num << '\n';
    for (int i = 0; i < 32; i++) {
        conv_bias[conv_bias_num] = (data_type_w)conv_2_bias2D[i];
        conv_bias_num++;
    }
    in_number_conv++;

    // Prepare weights and bias for conv layer 3
    memset(conv_3_weight2D, 0, 51200 * sizeof(float));
    load_weight_conv(
        weight_src, 
        conv_3_weight2D,
        weight_bias_record,
        nn_channel_size_conv, 
        nn_in_number_conv,
        nn_out_number_conv,
        in_number_conv);
    //cout << "Loading conv weight 3 to memory space, starting at: " <<conv_weight_num << '\n';
    for (int i = 0; i < 51200; i++) {
        conv_weight[conv_weight_num] = (data_type_w)conv_3_weight2D[i];
        conv_weight_num++;
    }
    memset(conv_3_bias2D, 0, 64 * sizeof(float));
    load_bias_conv(
        weight_src, 
        conv_3_bias2D,
        weight_bias_record,
        nn_channel_size_conv, 
        nn_in_number_conv,
        nn_out_number_conv,
        in_number_conv);
    //cout << "Loading conv bias 3 to memory space, starting at: " <<conv_bias_num << '\n';
    for (int i = 0; i < 64; i++) {
        conv_bias[conv_bias_num] = (data_type_w)conv_3_bias2D[i];
        conv_bias_num++;
    }
    in_number_conv++;

    //cout<<"Finished loading conv weight into memory! Total: " <<conv_weight_num  << "... ... ..."<<endl;
    //cout<<"Finished loading conv bias into memory! Total: " <<conv_bias_num  << "... ... ..."<<endl;

    // Prepare weights and bias for fc layer 1
    memset(fc_1_weight2D, 0, 10240 * sizeof(float));
    load_weight_fc(
        weight_src, 
        fc_1_weight2D,
        weight_bias_record,
        nn_channel_size_fc, 
        nn_in_number_fc,
        nn_out_number_fc,
        in_number_fc);
    //cout << "Loading fc weight 1 to memory space, starting at: " <<conv_weight_num << '\n';
    for (int i = 0; i < 10240; i++) {
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
    //cout << "Loading fc bias 1 to memory space, starting at: " <<conv_bias_num << '\n';
    for (int i = 0; i < 10; i++) {
        conv_bias[conv_bias_num] = (data_type_w)fc_1_bias2D[i];
        conv_bias_num++;
    }
    in_number_fc++;

    //---------------------conv weight bram ------------------------------------//
    //conv_1_w_load
    w_buf_t_load(conv_weight_tmp, conv_1_weight2D, conv_param_1[10], conv_param_1[1], conv_param_1[0], conv_param_1[2], w_r_offset, w_c_offset);
    //conv_2_w_load
    w_c_offset = 5;
    acc_param_conv_2[10] = w_c_offset;
    w_buf_t_load(conv_weight_tmp, conv_2_weight2D, conv_param_2[10], conv_param_2[1], conv_param_2[0], conv_param_2[2], w_r_offset, w_c_offset);
    //conv_3_w_load
    w_c_offset = 5+4*5;
    acc_param_conv_3[10] = w_c_offset;
    w_buf_t_load(conv_weight_tmp, conv_3_weight2D, conv_param_3[10], conv_param_3[1], conv_param_3[0], conv_param_3[2], w_r_offset, w_c_offset);
    //fc_w_load
    w_r_offset = 5;
    w_c_offset = 0;
    acc_param_conv_3[10] = w_c_offset;
    w_buf_t_load(conv_weight_tmp, fc_1_weight2D, conv_param_4[10], conv_param_4[1], conv_param_4[0], conv_param_4[2], w_r_offset, w_c_offset);
    w_buf_t.open("netOutput/weight.txt", ios::app);
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 32; j++) {
        w_buf_t <<"w_buf_data: "<<i<<"_"<<j<< endl;
        for(int k = 0; k < 32; k++){
          for(int l = 0; l < 30; l++){
            w_buf_t << conv_weight_tmp[i][j][k*32+l] << " ";
          }
          w_buf_t << endl;
        }
        w_buf_t << endl;
      }
      w_buf_t << endl;
    }
    w_buf_t.close();
    //write data to DDR_SH_ADDR
    //weight
    for(loop_var = 0; loop_var < 8; loop_var++){
        for(loop_var_1 = 0; loop_var_1 < 32; loop_var_1++){
            Fill_Bram(pci_bar_handle_4, 0xE01000000+0x00100000*loop_var+0x00001000*loop_var_1, conv_weight_tmp[loop_var][loop_var_1], 32*5);
        }  
    }
    //in_data
    Fill_Bram(pci_bar_handle_4, DDR_A_ADDR_1, in_data, 3*36*36);
    Fill_Bram(pci_bar_handle_4, DDR_B_ADDR, conv_bias, 32+32+64+10);
    
    printf("Finished writing to SH_DDR data\n");
    //cout<<"Finished loading fc weight into memory! Total: " <<conv_weight_num  << "... ... ..."<<endl;
    //cout<<"Finished loading fc bias into memory! Total: " <<conv_bias_num  << "... ... ..."<<endl;

//---------------------conv parameter bram transmission---------------------// 
    //cout << "Finished filling conv acc parameter into param bram!" << endl;

//---------------------conv 1 weight bram ------------------------------------//
    //nn_in_number_conv[in_number_conv]*nn_out_number_conv[in_number_conv]*nn_channel_size_conv[in_number_conv]*nn_channel_size_conv[in_number_conv]
    start_time = cycles_to_microseconds(ticks());
    //input
    set_cdma(pci_bar_handle,0x02000000,0x0000000C,0x02000000,0x00000000,0x00003000);
    //----------------------inference net ip status check -----------------------//    
    //1_1
    Fill_param(pci_bar_handle_4, CONV_CORE_PARAM, acc_param_conv_1, 16);
    Fill_param(pci_bar_handle_4, POOL_CORE_PARAM, acc_param_pool_1, 16); 
    Fill_Bram(pci_bar_handle_4, CONV_B_BRAM_PCIS, conv_bias, 32+32+64+10);
    end_time = cycles_to_microseconds(ticks());
    cout << "input data loading time = " << end_time-start_time << "  us" << endl;

    //compute
    start_time = cycles_to_microseconds(ticks());
    acc_call(pci_bar_handle,InstancePtr,acc_param_conv_1,CONV_CORE_PARAM,conv_param_1[5],conv_param_1[6]);
    end_time = cycles_to_microseconds(ticks());
    cout << "conv_1 processing time = " << end_time-start_time << "  us" << endl;

    start_time = cycles_to_microseconds(ticks());
        for(loop_var = 0; loop_var < 1; loop_var++){
            set_cdma(pci_bar_handle,0x02100000+loop_var*0x00001000,0x00000000,0x02000000+loop_var*0x00001000,0x00000000,0x000000080);
        } 
        /*Fill_param(pci_bar_handle_4, CONV_CORE_PARAM, acc_param_conv_4, 16); 
        Fill_param(pci_bar_handle_4, POOL_CORE_PARAM, acc_param_pool_4, 16); 
        XInference_net_Start(pci_bar_handle, &InstancePtr);
        
        while (!XInference_net_IsDone(pci_bar_handle, &InstancePtr)) {
            count++;
        }*/
    end_time = cycles_to_microseconds(ticks());
    cout << "test time = " << end_time-start_time << "  us" << endl;

    //----------------------conv_2 layer -----------------------// 
    Fill_param(pci_bar_handle_4, POOL_CORE_PARAM, acc_param_pool_2, 16); 
    for(int i=0;i<4;i++){
        //2_1
        //loading data
        start_time = cycles_to_microseconds(ticks());
        //load conv1 out_data
        //set_cdma(pci_bar_handle,0x02100000,0x00000000,0x02008000,0x00000000,0x00000200);
        for(loop_var = 0; loop_var < 8; loop_var++){
            set_cdma(pci_bar_handle,0x02100000+loop_var*0x00001000,0x00000000,0x02000000+loop_var*0x00001000,0x00000000,0x00000800);
        } 
        end_time = cycles_to_microseconds(ticks());
        cout << "conv2_1 loading time = " << end_time-start_time << "  us" << endl;

        //compute
        start_time = cycles_to_microseconds(ticks());
        acc_call(pci_bar_handle,InstancePtr,acc_param_conv_2,CONV_CORE_PARAM,conv_param_2[5],conv_param_2[6]);
        end_time = cycles_to_microseconds(ticks());
        cout << "conv_2_1 processing time = " << end_time-start_time << "  us" << endl;

        //2_2
        //compute
        start_time = cycles_to_microseconds(ticks());
        acc_call(pci_bar_handle,InstancePtr,acc_param_conv_2,CONV_CORE_PARAM,conv_param_2[5],conv_param_2[6]);
        end_time = cycles_to_microseconds(ticks());
        cout << "conv_2_2 processing time = " << end_time-start_time << "  us" << endl;

        //2_3
        //compute
        start_time = cycles_to_microseconds(ticks());
        acc_call(pci_bar_handle,InstancePtr,acc_param_conv_2,CONV_CORE_PARAM,conv_param_2[5],conv_param_2[6]);
        end_time = cycles_to_microseconds(ticks());
        cout << "conv_2_3 processing time = " << end_time-start_time << "  us" << endl;

        //2_4
        //compute
        start_time = cycles_to_microseconds(ticks());
        acc_call(pci_bar_handle,InstancePtr,acc_param_conv_2,CONV_CORE_PARAM,conv_param_2[5],conv_param_2[6]);
        end_time = cycles_to_microseconds(ticks());
        cout << "conv_2_4 processing time = " << end_time-start_time << "  us" << endl;
    }
    
    //----------------------conv_3 layer -----------------------// 
    Fill_param(pci_bar_handle_4, POOL_CORE_PARAM, acc_param_pool_3, 16); 
    for(int i=0;i<4;i++){
        //3_1
        //loading data
        start_time = cycles_to_microseconds(ticks());
        //load conv1 out_data
        //set_cdma(pci_bar_handle,0x02100000,0x00000000,0x02008000,0x00000000,0x00000200);
        for(loop_var = 0; loop_var < 8; loop_var++){
            set_cdma(pci_bar_handle,0x02100000+loop_var*0x00001000,0x00000000,0x02000000+loop_var*0x00001000,0x00000000,0x00000300);
        } 
        end_time = cycles_to_microseconds(ticks());
        cout << "conv3_1 loading time = " << end_time-start_time << "  us" << endl;

        //compute
        start_time = cycles_to_microseconds(ticks());
        acc_call(pci_bar_handle,InstancePtr,acc_param_conv_3,CONV_CORE_PARAM,conv_param_3[5],conv_param_3[6]);
        end_time = cycles_to_microseconds(ticks());
        cout << "conv_3_1 processing time = " << end_time-start_time << "  us" << endl;

        //3_2
        //compute
        start_time = cycles_to_microseconds(ticks());
        acc_call(pci_bar_handle,InstancePtr,acc_param_conv_3,CONV_CORE_PARAM,conv_param_3[5],conv_param_3[6]);
        end_time = cycles_to_microseconds(ticks());
        cout << "conv_3_2 processing time = " << end_time-start_time << "  us" << endl;

    }
    
    //----------------------fc layer -----------------------// 
    Fill_param(pci_bar_handle_4, POOL_CORE_PARAM, acc_param_pool_4, 16); 
    for(int i=0;i<8;i++){
        //loading data
        start_time = cycles_to_microseconds(ticks());
        //load conv1 out_data
        //set_cdma(pci_bar_handle,0x02100000,0x00000000,0x02008000,0x00000000,0x00000200);
        for(loop_var = 0; loop_var < 8; loop_var++){
            set_cdma(pci_bar_handle,0x02100000+loop_var*0x00001000,0x00000000,0x02000000+loop_var*0x00001000,0x00000000,0x00000080);
        } 
        end_time = cycles_to_microseconds(ticks());
        cout << "fc loading time = " << end_time-start_time << "  us" << endl;

        //compute
        start_time = cycles_to_microseconds(ticks());
        acc_call(pci_bar_handle,InstancePtr,acc_param_conv_4,CONV_CORE_PARAM,conv_param_4[5],conv_param_4[6]);
        end_time = cycles_to_microseconds(ticks());
        cout << "fc processing time = " << end_time-start_time << "  us" << endl;

    }
    
    Read_Bram(pci_bar_handle_4, BUF_OUT_1_0, out_temp_1, 1);
    Read_Bram(pci_bar_handle_4, BUF_OUT_1_1, out_temp_2, 1);
    Read_Bram(pci_bar_handle_4, BUF_OUT_1_2, out_temp_3, 1);
    Read_Bram(pci_bar_handle_4, BUF_OUT_1_3, out_temp_4, 1);
    Read_Bram(pci_bar_handle_4, BUF_OUT_1_4, out_temp_5, 1);
    Read_Bram(pci_bar_handle_4, BUF_OUT_1_5, out_temp_6, 1);
    Read_Bram(pci_bar_handle_4, BUF_OUT_1_6, out_temp_7, 1);
    Read_Bram(pci_bar_handle_4, BUF_OUT_1_7, out_temp_8, 1);
    Read_Bram(pci_bar_handle_4, BUF_OUT_1_8, out_temp_9, 1);
    Read_Bram(pci_bar_handle_4, BUF_OUT_1_9, out_temp_10, 1);
    fc_3_out[0]=(float)(out_temp_1[0]);
    fc_3_out[1]=(float)(out_temp_2[0]);
    fc_3_out[2]=(float)(out_temp_3[0]);
    fc_3_out[3]=(float)(out_temp_4[0]);
    fc_3_out[4]=(float)(out_temp_5[0]);
    fc_3_out[5]=(float)(out_temp_6[0]);
    fc_3_out[6]=(float)(out_temp_7[0]);
    fc_3_out[7]=(float)(out_temp_8[0]);
    fc_3_out[8]=(float)(out_temp_9[0]);
    fc_3_out[9]=(float)(out_temp_10[0]);
    for (loop_var = 0; loop_var < 10; loop_var++ ) {
        cout << fc_3_out[loop_var] << "  ";
    }
    cout << endl;
    softmax(fc_3_out, 10);
    predict(fc_3_out, 10);
    gettimeofday(&end,0);
    diff = 1000000*(end.tv_sec-start.tv_sec) + end.tv_usec-start.tv_usec;
    cout << "Cifarnet processing time = " << diff << "  us" << endl;

//------------------------------------------------------------------------------------------
    printf("\n");
    printf("Reading and verifying DDR_B Dst Buffer 1KB\n");

    /*for ( loop_var = 0; loop_var < 256; loop_var++ ) {
        rc_4 = fpga_pci_peek(pci_bar_handle_4, (DDR_B_ADDR + loop_var*4), &value);
       fail_on(rc_4, out, "Unable to read read from the fpga !");
       //printf("register: 0x%x\n", value);
       if (value != loop_var)
        {
          printf("Data mismatch!");
        }
    }
    printf("\n");
    printf("CDMA Transfer Successful!\n");*/


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
