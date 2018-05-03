//
// Created by yaochen on 9/4/18.
//

#ifndef _CONSTRUCT_NET_H_
#define _CONSTRUCT_NET_H_

#include "construct_layer.h"

//int inference_net(int * argc, char * argv) {
int inference_net(int rc, int fd, XInference_net * InstancePtr, int layer_count,
			int loop_param[][18], int conv_param[][16], 
			int acc_param_conv[][16], int acc_param_pool[][16], 
			float * in_data_reordered, float * in_data, float * out_data, float * conv_bias) {
	
    // stack the layers here
    // conv_pool_layer()
    // conv_layer()
    // fc_layer()

    //data_buf matrix
    float *in_data_buf = NULL; 
    float *out_data_buf = NULL;
    
    //the net largest loop size
    in_data_buf = (float *) malloc (Tn * IBUF_t * IBUF_t * loop_param[2][0] * loop_param[2][4] * loop_param[2][5] * sizeof(float));
    out_data_buf = (float *) malloc (Tm * OBUF_t * OBUF_t * loop_param[2][0] * loop_param[2][6] * loop_param[2][7] * sizeof(float));

    int bias_count = 0;
    int data_turn = 0;
    int K_max_last_r = 0;

    for (int layer_var = 0; layer_var < layer_count; layer_var ++) {

    	//load layer bias
    	Fill_Bram(pci_bar_handle_4, CONV_B_BRAM_PCIS, &conv_bias[bias_count], conv_param[layer_var][2]);
    	bias_count += conv_param[layer_var][2];
    
    	//layer_process
    	if (data_turn == 0) {
	    	if (layer_var == 0) {
    			//the 1st layer don't need to reorder in_data
	    		conv_pool_layer(rc, fd, InstancePtr, loop_param[layer_var], conv_param[layer_var], 
					acc_param_pool[layer_var], acc_param_conv[layer_var], in_data_reordered, out_data_buf, K_max_last_r);
				
	    	} else {
	    		//reorder in_data
	    		in_data_reorder(loop_param[layer_var], acc_param_conv[layer_var], in_data_buf, in_data);
			    
				conv_pool_layer(rc, fd, InstancePtr, loop_param[layer_var], conv_param[layer_var], 
					acc_param_pool[layer_var], acc_param_conv[layer_var], in_data_buf, out_data_buf, K_max_last_r);
				
	    	}
				//reorder out_data
				out_data_reorder(loop_param[layer_var], acc_param_pool[layer_var], out_data, out_data_buf);
			    
				data_turn = 1;
    	}
		else {
			//reorder in_data
			in_data_reorder(loop_param[layer_var], acc_param_conv[layer_var], in_data_buf, out_data);
		    
			conv_pool_layer(rc, fd, InstancePtr, loop_param[layer_var], conv_param[layer_var], 
				acc_param_pool[layer_var], acc_param_conv[layer_var], in_data_buf, out_data_buf, K_max_last_r);
			
			//reorder out_data
			out_data_reorder(loop_param[layer_var], acc_param_pool[layer_var], in_data, out_data_buf);
		    
			data_turn = 0;
		}
    }

    if (in_data_buf != NULL) {
    	free(in_data_buf);
    }
	if (out_data_buf != NULL) {
    	free(out_data_buf);
    }

}

#endif //SOFTWARE_YANG_CONSTRUCT_NET_H
