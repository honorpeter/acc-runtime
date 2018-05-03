#ifndef _ACC_INSTANCE_H_
#define _ACC_INSTANCE_H_

#include "max_pool_acc_innerpp.h"
#include "acc_dim_config.h"

max_pool_acc<data_type, data_type_w, data_type_o, 16, 16, 16, 2, 3> maxPoolAcc1;

void max_pool_layer_new(
   int R_in,
   int C_in,
   int N,
   int K,
   int R,
   int C,
   int S,
   int P,
   bool act,
   data_type *in_data_1,
   data_type *in_data_2,
   data_type *in_data_3,
   data_type *in_data_4,
   data_type *in_data_5,
   data_type *in_data_6,
   data_type *in_data_7,
   data_type *in_data_8,
   data_type_o *out_data_1,
   data_type_o *out_data_2,
   data_type_o *out_data_3,
   data_type_o *out_data_4,
   data_type_o *out_data_5,
   data_type_o *out_data_6,
   data_type_o *out_data_7,
   data_type_o *out_data_8) {

   maxPoolAcc1.max_pool_layer_acc(R_in, C_in, N, K, R, C, S, P, act, in_data_1, in_data_2, in_data_3, in_data_4, in_data_5, in_data_6, in_data_7, in_data_8, out_data_1, out_data_2, out_data_3, out_data_4, out_data_5, out_data_6, out_data_7, out_data_8);

}



#endif