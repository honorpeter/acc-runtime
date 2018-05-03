//
// Created by yaochen on 9/4/18.
//

/*TODO: complete an initial conv_pool_layer with existing small models
 *TODO: add the simple scheduling method with this function
 */

#ifndef _CONSTRUCT_LAYER_H_
#define _CONSTRUCT_LAYER_H_

#include <unistd.h>
#include "../inference_net_hls/acc_dim_config.h"
#include "acc_mem_config.h"

int conv_pool_layer(int * param, data_type_w * in_weight, data_type * in_data, data_type_o out_data);
int conv_layer(int * param, data_type_w * in_weight, data_type * in_data, data_type_o out_data);
int pool_layer(int * param, data_type_w * in_weight, data_type * in_data, data_type_o out_data);
int fc_layer(int * param, data_type_w * in_weight, data_type * in_data, data_type_o out_data);


int conv_pool_layer(int rc, int fd, XInference_net * InstancePtr, 
		int * loop_param, int * conv_param, 
		int * acc_param_pool, int * acc_param_conv, 
		float * in_data, float * out_data, int& K_max_last_r) {

    // iterations of accelerator calls, the scheduling should be implemented here
    // this function is similar to the function in construct_layer.h
    // the layer call models could be added in this header file as well
    int iter_n = loop_param[0];
    int iter_m = loop_param[1];
    int iter_r_in = loop_param[2];
    int iter_c_in = loop_param[3];
    int iter_r_in_buf = loop_param[4];
    int iter_c_in_buf = loop_param[5];
    int iter_r_out_buf = loop_param[6];
    int iter_c_out_buf = loop_param[7];
    
    int offset_n = loop_param[8];
    int offset_m = loop_param[9];
    int offset_r = Tr;
    int offset_c = Tc;
    
    int w_r_offset = 0;
    int w_c_offset = 0;

    K_max_last_r = (K_max_last_r > conv_param[1] ? K_max_last_r : conv_param[1]);

    cout << "iter_n: " << iter_n << endl;
    cout << "iter_m: " << iter_m << endl;
    cout << "iter_r_in: " << iter_r_in << endl;
    cout << "iter_c_in: " << iter_c_in << endl;
    cout << "iter_r_in_buf: " << iter_r_in_buf << endl;
    cout << "iter_c_in_buf: " << iter_c_in_buf << endl;
    cout << "iter_r_out_buf: " << iter_r_out_buf << endl;
    cout << "iter_c_out_buf: " << iter_c_out_buf << endl;
    cout << "offset_n: " << offset_n << endl;
    cout << "offset_m: " << offset_m << endl;
    cout << "offset_r: " << offset_r << endl;
    cout << "offset_c: " << offset_c << endl;
    cout << "K_max_last_r: " << K_max_last_r << endl;
    cout << endl;

	//load_acc_param_pool
    Fill_param(pci_bar_handle_4, POOL_CORE_PARAM, acc_param_pool, 16);

    for (int r_out_buf = 0; r_out_buf < iter_r_out_buf; r_out_buf ++) {
		for (int c_out_buf = 0; c_out_buf < iter_c_out_buf; c_out_buf ++) {
			for (int r_in_buf = 0; r_in_buf < iter_r_in_buf; r_in_buf ++) {
				for (int c_in_buf = 0; c_in_buf < iter_c_in_buf; c_in_buf ++) {
				    for (int m = 0; m < iter_m; m ++) {
				    	for (int n = 0; n < iter_n; n ++) {

				    		//write data to in_buf
				    		if (acc_param_conv[12] == 0) {
				    			//load in_buf_0_0
				    			for (int channel = 0; channel < offset_n; channel ++) {
				    				size_t write_offset = 0;
				        			while (write_offset < IBUF_t * IBUF_t * 4) {
				            			rc = pwrite(fd,
				                			in_data + (r_in_buf * iter_c_in_buf + c_in_buf) * iter_n * offset_n * IBUF_t * IBUF_t 
				                				+ n * offset_n * IBUF_t * IBUF_t
				                				+ channel * IBUF_t * IBUF_t + write_offset / 4,
				                			IBUF_t * IBUF_t * 2,
				                			BUF_IN_0_0 + channel * IBUF_t * IBUF_t * 4 + write_offset);
				            			if (rc < 0) {
				                			fail_on((rc = (rc < 0)? errno:0), out, "call to pwrite failed.");
				            			}
				            			write_offset += rc;
				        			}
				        			rc = 0;
				    			}

				    			rc = fsync(fd);
				    			if (rc < 0) {
				        			cout << "fsync error!" << endl;
				    			}
				    		}
				    		else {
				    			//load in_buf_1_0
				    			for (int channel = 0; channel < offset_n; channel ++) {
				    				size_t write_offset = 0;
				        			while (write_offset < IBUF_t * IBUF_t * 4) {
				            			rc = pwrite(fd,
				                			in_data + (r_in_buf * iter_c_in_buf + c_in_buf) * iter_n * offset_n * IBUF_t * IBUF_t 
				                				+ n * offset_n * IBUF_t * IBUF_t
				                				+ channel * IBUF_t * IBUF_t + write_offset / 4,
				                			IBUF_t * IBUF_t * 2,
				                			BUF_IN_1_0 + channel * IBUF_t * IBUF_t * 4 + write_offset);
				            			if (rc < 0) {
				                			fail_on((rc = (rc < 0)? errno:0), out, "call to pwrite failed.");
				            			}
				            			write_offset += rc;
				        			}
				        			rc = 0;
				    			}

				    			rc = fsync(fd);
				    			if (rc < 0) {
				        			cout << "fsync error!" << endl;
				    			}
				    		}
						    
						    for (int r_in = 0; r_in < iter_r_in; r_in ++) {
								for (int c_in = 0; c_in < iter_c_in; c_in ++) {
				    				//change n_offset
				    				acc_param_conv[1] = offset_n * n;
						    		//load_acc_param_conv
						    		acc_param_conv[5] = offset_r * r_in;
						    		acc_param_conv[6] = offset_c * c_in;
									Fill_param(pci_bar_handle_4, CONV_CORE_PARAM, acc_param_conv, 16);

									//compute
									XInference_net_Start(pci_bar_handle, InstancePtr);
									while (!XInference_net_IsDone(pci_bar_handle, InstancePtr)) {
										
									}
						    	}
				    		}

				    		//change w_offset
							if (acc_param_conv[10] + 2 * acc_param_conv[4] > WBUF_t) {
								w_c_offset = 0;
								w_r_offset = K_max_last_r;
								K_max_last_r = 0;
							} else {
								w_c_offset = acc_param_conv[4];
							} 
							acc_param_conv[9] += w_r_offset; //w_r_offset
							acc_param_conv[10] += w_c_offset;//w_c_offset

				    		//change in_buf_flag
				    		if (acc_param_conv[12] == 0) {
				    			acc_param_conv[12] = 1;
				    		}
				    		else {
				    			acc_param_conv[12] = 0;
				    		}
				    	}

				    	//read data from out_buf & change out_buf_flag
				    	if (acc_param_conv[13] == 0) {
				    		
				    		//load out_buf_0_0
				    		for (int channel = 0; channel < offset_m; channel ++) {
				    			size_t read_offset = 0;
				        		while (read_offset < OBUF_t * OBUF_t * 4) {
				            		rc = pread(fd,
				            			out_data + (r_out_buf * iter_c_out_buf + c_out_buf) * iter_m * offset_m * OBUF_t * OBUF_t 
				                				+ m * offset_m * OBUF_t * OBUF_t
				                				+ channel * OBUF_t * OBUF_t + read_offset / 4,
				                		//out_data + channel * OBUF_t * OBUF_t + read_offset / 4,
				                		OBUF_t * OBUF_t * 2,
				                		BUF_OUT_1_0 + channel * OBUF_t * OBUF_t * 4 + read_offset);
				            		if (rc < 0) {
				                		fail_on((rc = (rc < 0)? errno:0), out, "call to pread failed.");
				            		}
				            		read_offset += rc;
				        		}
				        		rc = 0;
				    		}
				    		acc_param_conv[13] = 1;

				    	}
				    	else {
				    		
				    		//load out_buf_1_0
				    		for (int channel = 0; channel < offset_m; channel ++) {
				    			size_t read_offset = 0;
				        		while (read_offset < OBUF_t * OBUF_t * 4) {
				            		rc = pread(fd,
				            			out_data + (r_out_buf * iter_c_out_buf + c_out_buf) * iter_m * offset_m * OBUF_t * OBUF_t 
				                				+ m * offset_m * OBUF_t * OBUF_t
				                				+ channel * OBUF_t * OBUF_t + read_offset / 4,
				                		//out_data + channel * OBUF_t * OBUF_t + read_offset / 4,
				                		OBUF_t * OBUF_t * 2,
				                		BUF_OUT_2_0 + channel * OBUF_t * OBUF_t * 4 + read_offset);
				            		if (rc < 0) {
				                		fail_on((rc = (rc < 0)? errno:0), out, "call to pread failed.");
				            		}
				            		read_offset += rc;
				        		}
				        		rc = 0;
				    		}
				    		acc_param_conv[13] = 1;

				    	}
				    }

				    //change in_buf_flag
				    if (acc_param_conv[12] == 0) {
				    	acc_param_conv[12] = 1;
				    }
				    else {
				    	acc_param_conv[12] = 0;
				    }
				}
			}
		}
	}
out:
	

    return 0;

}

#endif