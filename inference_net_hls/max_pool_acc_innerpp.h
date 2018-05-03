#ifndef _MAX_POOL_ACC_H_
#define _MAX_POOL_ACC_H_

#include <iostream>
#include <fstream>
#include "activation_functions.h"

#if _C_DEBUG_MODE_
//#include <algorithm>
#endif

using namespace std;

template <typename T, typename W, typename G, int Tn, int Tr, int Tc, int S_max, int K_max>
class max_pool_acc {

private:
	int pool_layer_number;

public:
	max_pool_acc() : pool_layer_number(0) {pool_layer_number = 0;};

	////------------------------------C++ debugging functions---------------------------------------////
	// Reset output buffer
	void out_buf_reset(G buf[][Tr][Tc]){
        for(int i = 0; i < Tn; i++){
            for(int j = 0; j < Tr; j++){
                for(int k = 0; k < Tc; k++){
                    buf[i][j][k] = G(0);
				}
			}
		}
	}
    ////-----------------------------Accelerator Functions---------------------------------------////
    // Load input data
    void in_buf_load(T buf[][(Tr-1)*S_max + K_max][(Tc-1)*S_max + K_max],T *in_data_1,T *in_data_2,T *in_data_3,T *in_data_4,T *in_data_5,T *in_data_6,T *in_data_7,T *in_data_8, int n, int r, int c, int S, int K, int P, int R, int C, int N, int R_IN, int C_IN, int TR, int TC) {
        for (int j = r * S - P; j < r * S + TR - P; j++) {
            for (int k = c * S - P; k < c * S + TC - P; k++) {
#pragma HLS PIPELINE
        		 for (int i = n; i < n + Tn; i+=8){
#pragma HLS UNROLL
                    if ((n + Tn > N && i + 0 >= N ) || j < 0 || j >= (R_IN - 2 * P) || k < 0 || k >= (C_IN - 2 * P)) {
                        buf[i + 0 - n][j - r * S + P][k - c * S + P] = T(0);
                    }else {
                        buf[i + 0 - n][j - r * S + P][k - c * S + P] = *(in_data_1 + i/8 * (R_IN - 2 * P) * (C_IN - 2 * P) + j * (C_IN - 2 * P) + k);
                    }
                    if ((n + Tn > N && i + 1 >= N ) || j < 0 || j >= (R_IN - 2 * P) || k < 0 || k >= (C_IN - 2 * P)) {
                        buf[i + 1 - n][j - r * S + P][k - c * S + P] = T(0);
                    }else {
                        buf[i + 1 - n][j - r * S + P][k - c * S + P] = *(in_data_2 + i/8 * (R_IN - 2 * P) * (C_IN - 2 * P) + j * (C_IN - 2 * P) + k);
                    }
                    if ((n + Tn > N && i + 2 >= N ) || j < 0 || j >= (R_IN - 2 * P) || k < 0 || k >= (C_IN - 2 * P)) {
                        buf[i + 2 - n][j - r * S + P][k - c * S + P] = T(0);
                    }else {
                        buf[i + 2 - n][j - r * S + P][k - c * S + P] = *(in_data_3 + i/8 * (R_IN - 2 * P) * (C_IN - 2 * P) + j * (C_IN - 2 * P) + k);
                    }
                    if ((n + Tn > N && i + 3 >= N ) || j < 0 || j >= (R_IN - 2 * P) || k < 0 || k >= (C_IN - 2 * P)) {
                        buf[i + 3 - n][j - r * S + P][k - c * S + P] = T(0);
                    }else {
                        buf[i + 3 - n][j - r * S + P][k - c * S + P] = *(in_data_4 + i/8 * (R_IN - 2 * P) * (C_IN - 2 * P) + j * (C_IN - 2 * P) + k);
                    }
                    if ((n + Tn > N && i + 4 >= N ) || j < 0 || j >= (R_IN - 2 * P) || k < 0 || k >= (C_IN - 2 * P)) {
                        buf[i + 4 - n][j - r * S + P][k - c * S + P] = T(0);
                    }else {
                        buf[i + 4 - n][j - r * S + P][k - c * S + P] = *(in_data_5 + i/8 * (R_IN - 2 * P) * (C_IN - 2 * P) + j * (C_IN - 2 * P) + k);
                    }
                    if ((n + Tn > N && i + 5 >= N ) || j < 0 || j >= (R_IN - 2 * P) || k < 0 || k >= (C_IN - 2 * P)) {
                        buf[i + 5 - n][j - r * S + P][k - c * S + P] = T(0);
                    }else {
                        buf[i + 5 - n][j - r * S + P][k - c * S + P] = *(in_data_6 + i/8 * (R_IN - 2 * P) * (C_IN - 2 * P) + j * (C_IN - 2 * P) + k);
                    }
                    if ((n + Tn > N && i + 6 >= N ) || j < 0 || j >= (R_IN - 2 * P) || k < 0 || k >= (C_IN - 2 * P)) {
                        buf[i + 6 - n][j - r * S + P][k - c * S + P] = T(0);
                    }else {
                        buf[i + 6 - n][j - r * S + P][k - c * S + P] = *(in_data_7 + i/8 * (R_IN - 2 * P) * (C_IN - 2 * P) + j * (C_IN - 2 * P) + k);
                    }
                    if ((n + Tn > N && i + 7 >= N ) || j < 0 || j >= (R_IN - 2 * P) || k < 0 || k >= (C_IN - 2 * P)) {
                        buf[i + 7 - n][j - r * S + P][k - c * S + P] = T(0);
                    }else {
                        buf[i + 7 - n][j - r * S + P][k - c * S + P] = *(in_data_8 + i/8 * (R_IN - 2 * P) * (C_IN - 2 * P) + j * (C_IN - 2 * P) + k);
                    }
				}
			}
		}
	}
    // Max pooling computation kernel
    void pool_engine(T in_buf[][(Tr-1)*S_max + K_max][(Tc-1)*S_max + K_max], G out_buf[][Tr][Tc], int S, int n, int r, int c, int K, int R, int C, int TR, int TC){
        for(int i=0; i<K; i++){
            for(int j=0; j<K; j++){
                for(int tr=0; tr<Tr&&tr+r<R&&(S * tr + i)<TR; tr++){
                    for(int tc=0; tc<Tc&&tc+c<C&&(S * tc + j)<TC; tc++){
#pragma HLS PIPELINE
                        for(int tn=0; tn<Tn; tn++){
#pragma HLS UNROLL
                            out_buf[tn][tr][tc] = (i==0&&j==0)?in_buf[tn][S*tr][S*tc]:((out_buf[tn][tr][tc]>in_buf[tn][S*tr+i][S*tc+j])?out_buf[tn][tr][tc]:in_buf[tn][S*tr+i][S*tc+j]);
                        }
                    }
                }
            }
        }
    }
    // Ouput out_buf data to output interface
    void output_res(G out_buf[][Tr][Tc],G *out_data_1,G *out_data_2,G *out_data_3,G *out_data_4,G *out_data_5,G *out_data_6,G *out_data_7,G *out_data_8, int n, int r, int c, int N, int R, int C, bool act){
            for (int i = n; i < n + Tn; i += 8) {
                if (N < n + Tn && i == N) { break; }
                for (int j = r; j < r + Tr; j++) {
                    if (R < r + Tr && j == R) { break; }
                    for (int k = c; k < c + Tc; k++) {
                        if (C < c + Tc && k == C) { break; }
                        if (act) {
                        	if (i + 0 <N)
                            	*(out_data_1 + (i/8) * R * C + j * C + k) = relu(out_buf[i + 0 - n][j - r][k - c]);
                        	if (i + 1 <N)
                            	*(out_data_2 + (i/8) * R * C + j * C + k) = relu(out_buf[i + 1 - n][j - r][k - c]);
                        	if (i + 2 <N)
                            	*(out_data_3 + (i/8) * R * C + j * C + k) = relu(out_buf[i + 2 - n][j - r][k - c]);
                        	if (i + 3 <N)
                            	*(out_data_4 + (i/8) * R * C + j * C + k) = relu(out_buf[i + 3 - n][j - r][k - c]);
                        	if (i + 4 <N)
                            	*(out_data_5 + (i/8) * R * C + j * C + k) = relu(out_buf[i + 4 - n][j - r][k - c]);
                        	if (i + 5 <N)
                            	*(out_data_6 + (i/8) * R * C + j * C + k) = relu(out_buf[i + 5 - n][j - r][k - c]);
                        	if (i + 6 <N)
                            	*(out_data_7 + (i/8) * R * C + j * C + k) = relu(out_buf[i + 6 - n][j - r][k - c]);
                        	if (i + 7 <N)
                            	*(out_data_8 + (i/8) * R * C + j * C + k) = relu(out_buf[i + 7 - n][j - r][k - c]);
                        }
                        else {
                        	if (i + 0 <N)
                            	*(out_data_1+ (i/8) * R * C + j * C + k) = out_buf[i + 0 - n][j - r][k - c];
                        	if (i + 1 <N)
                            	*(out_data_2+ (i/8) * R * C + j * C + k) = out_buf[i + 1 - n][j - r][k - c];
                        	if (i + 2 <N)
                            	*(out_data_3+ (i/8) * R * C + j * C + k) = out_buf[i + 2 - n][j - r][k - c];
                        	if (i + 3 <N)
                            	*(out_data_4+ (i/8) * R * C + j * C + k) = out_buf[i + 3 - n][j - r][k - c];
                        	if (i + 4 <N)
                            	*(out_data_5+ (i/8) * R * C + j * C + k) = out_buf[i + 4 - n][j - r][k - c];
                        	if (i + 5 <N)
                            	*(out_data_6+ (i/8) * R * C + j * C + k) = out_buf[i + 5 - n][j - r][k - c];
                        	if (i + 6 <N)
                            	*(out_data_7+ (i/8) * R * C + j * C + k) = out_buf[i + 6 - n][j - r][k - c];
                        	if (i + 7 <N)
                            	*(out_data_8+ (i/8) * R * C + j * C + k) = out_buf[i + 7 - n][j - r][k - c];
                        }
                    }
                }
            }
    }
///////////////////////------------------conv accelerator----------------//////////////////////////
    void max_pool_layer_acc(
            int R_IN,// input Row
            int C_IN,// input column
            int N, //input feature number
            int K, //input kernel size
            int R, // output Row
            int C, // output column
            int S, // stride size
            int P, // padding size
            bool act, // activation function bit (1-- with act, 0--without act)
            T *in_data_1, // in_data[N][(R-1)*S + K][(C-1)*S + K] --> [N][(R-1)*S + K - 2*P][(C-1)*S + K - 2*P]
            T *in_data_2, // in_data[N][(R-1)*S + K][(C-1)*S + K] --> [N][(R-1)*S + K - 2*P][(C-1)*S + K - 2*P]
            T *in_data_3, // in_data[N][(R-1)*S + K][(C-1)*S + K] --> [N][(R-1)*S + K - 2*P][(C-1)*S + K - 2*P]
            T *in_data_4, // in_data[N][(R-1)*S + K][(C-1)*S + K] --> [N][(R-1)*S + K - 2*P][(C-1)*S + K - 2*P]
            T *in_data_5, // in_data[N][(R-1)*S + K][(C-1)*S + K] --> [N][(R-1)*S + K - 2*P][(C-1)*S + K - 2*P]
            T *in_data_6, // in_data[N][(R-1)*S + K][(C-1)*S + K] --> [N][(R-1)*S + K - 2*P][(C-1)*S + K - 2*P]
            T *in_data_7, // in_data[N][(R-1)*S + K][(C-1)*S + K] --> [N][(R-1)*S + K - 2*P][(C-1)*S + K - 2*P]
            T *in_data_8, // in_data[N][(R-1)*S + K][(C-1)*S + K] --> [N][(R-1)*S + K - 2*P][(C-1)*S + K - 2*P]
            G *out_data_1, // out[M][R][C]
            G *out_data_2, // out[M][R][C]
            G *out_data_3, // out[M][R][C]
            G *out_data_4, // out[M][R][C]
            G *out_data_5, // out[M][R][C]
            G *out_data_6, // out[M][R][C]
            G *out_data_7, // out[M][R][C]
            G *out_data_8){ // out[M][R][C]

        /***************local data buffer******************************/
        int TR=0;
        int TC=0;
        T in_buf_1[Tn][(Tr-1)*S_max + K_max][(Tc-1)*S_max + K_max];
        T in_buf_0[Tn][(Tr-1)*S_max + K_max][(Tc-1)*S_max + K_max];
        G out_buf_1[Tn][Tr][Tc];
        G out_buf_0[Tn][Tr][Tc];

        /***************Ptr and buffer initialization******************************/
        bool in_buf_0_empty = 1;
        bool in_buf_1_empty = 1;
        bool out_buf_0_empty = 1;
        bool out_buf_1_empty = 1;
        int loadbufPtr = 0;
        int combufPtr = 0;
        int resbufPtr = 0;

#if _HLS_MODE_
#pragma HLS ARRAY_PARTITION variable=in_buf_1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=in_buf_0 complete dim=1
#pragma HLS ARRAY_PARTITION variable=out_buf_1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=out_buf_0 complete dim=1
#endif

#if _C_DEBUG_MODE_
#if _KERNEL_DEBUG_
            //cout << "Starting max_pool_acc_innerpp layer ...." << endl;
            //buffer local data initiallization: must do it in C++ debug!
            out_buf_reset(out_buf_1);
            out_buf_reset(out_buf_0);
#endif
#endif
        for(int r = 0; r < R; r += Tr){
            for(int c = 0; c < C; c += Tc){
                TR=((r * S + (Tr - 1) * S + K)>R_IN?(R_IN - r * S):((Tr - 1) * S + K));
                TC=((c * S + (Tc - 1) * S + K)>C_IN?(C_IN - c * S):((Tc - 1) * S + K));
                    for(int n = 0; n < N; n += 2*Tn){
   //--------------------------Load input B W D in ping-pong manner-------------------------//
						while (in_buf_0_empty | in_buf_1_empty) {
							if (loadbufPtr == 1) {
                    			//cout << "loading input buffer 1...." << endl;
                        		// load input data
                        		in_buf_load(in_buf_1, in_data_1, in_data_2, in_data_3, in_data_4, in_data_5, in_data_6, in_data_7, in_data_8, n+Tn, r, c, S, K, P, R, C, N, R_IN, C_IN, TR, TC);
                        		in_buf_1_empty = 0;
                        		//cout << "buffer 1 full" << endl;
                        		loadbufPtr = 0;
                        	} else {
                    			//cout << "loading input buffer 0...." << endl;
                        		// load input data
                        		in_buf_load(in_buf_0, in_data_1, in_data_2, in_data_3, in_data_4, in_data_5, in_data_6, in_data_7, in_data_8, n, r, c, S, K, P, R, C, N, R_IN, C_IN, TR, TC);
                        		in_buf_0_empty = 0;
                        		//cout << "buffer 0 full" << endl;
                        		loadbufPtr = 1;
							}
                       }
                       loadbufPtr = 0;
  //------------------------------compute buffered data -----------------------------------//
                    	while (!in_buf_0_empty | !in_buf_1_empty) {
                    		if (combufPtr == 1) {
                    			//cout << "computing input buffer 1...." << endl;
                        		pool_engine(in_buf_1, out_buf_1, S, n+Tn, r, c, K, R, C, TR, TC);
                    			out_buf_1_empty = 0;
                    			in_buf_1_empty = 1;
                    			combufPtr = 0;
                    			//cout << "buffer 1 computed" << endl;
                    		} else {
                    			//cout << "computing input buffer 0...." << endl;
                        		pool_engine(in_buf_0, out_buf_0, S, n, r, c, K, R, C, TR, TC);
                    			out_buf_0_empty = 0;
                    			in_buf_0_empty = 1;
                    			combufPtr = 1;
                    			//cout << "buffer 0 computed" << endl;
							}
                       }
                       combufPtr = 0;
  //---------------------------transfer output data----------------------------------------//
                    	while (!out_buf_0_empty | !out_buf_1_empty) {
                    		if (resbufPtr == 1) {
                    			//cout << "output buffer 1...." << endl;
                    			// transfer output data
                    			output_res(out_buf_1, out_data_1, out_data_2, out_data_3, out_data_4, out_data_5, out_data_6, out_data_7, out_data_8, n+Tn, r, c, N, R, C, act);
                    			out_buf_1_empty = 1;
                    			resbufPtr = 0;
                    			//cout << "buffer 1 res" << endl;
                    		} else {
                    			//cout << "output buffer 0...." << endl;
                    			// transfer output data
                    			output_res(out_buf_0, out_data_1, out_data_2, out_data_3, out_data_4, out_data_5, out_data_6, out_data_7, out_data_8, n, r, c, N, R, C, act);
                    			out_buf_0_empty = 1;
                    			resbufPtr = 1;
								//cout << "buffer 0 res" << endl;
							}
						}
						resbufPtr = 0;
					}
				}
			}
#if _C_DEBUG_MODE_
#if _KERNEL_DEBUG_
            //cout << "Finished max_pool_acc_innerpp layer ...." << endl;
            /*ofstream max_pool_out;
            max_pool_out.open("max_pool_out_data.txt",ios::app);
            max_pool_out <<"max_pool output: "<< endl;
            for (int i = 0; i < N/8; i++) {
                for (int j = 0; j < R; j++) {
                    for(int k = 0; k < C; k++){
                        max_pool_out << *(out_data_1 + i*R*C + j*C + k) << " ";
                    }max_pool_out << endl;
                }max_pool_out << endl;
                for (int j = 0; j < R; j++) {
                    for(int k = 0; k < C; k++){
                        max_pool_out << *(out_data_2 + i*R*C + j*C + k) << " ";
                    }max_pool_out << endl;
                }max_pool_out << endl;
                for (int j = 0; j < R; j++) {
                    for(int k = 0; k < C; k++){
                        max_pool_out << *(out_data_3 + i*R*C + j*C + k) << " ";
                    }max_pool_out << endl;
                }max_pool_out << endl;
                for (int j = 0; j < R; j++) {
                    for(int k = 0; k < C; k++){
                        max_pool_out << *(out_data_4 + i*R*C + j*C + k) << " ";
                    }max_pool_out << endl;
                }max_pool_out << endl;
                for (int j = 0; j < R; j++) {
                    for(int k = 0; k < C; k++){
                        max_pool_out << *(out_data_5 + i*R*C + j*C + k) << " ";
                    }max_pool_out << endl;
                }max_pool_out << endl;
                for (int j = 0; j < R; j++) {
                    for(int k = 0; k < C; k++){
                        max_pool_out << *(out_data_6 + i*R*C + j*C + k) << " ";
                    }max_pool_out << endl;
                }max_pool_out << endl;
                for (int j = 0; j < R; j++) {
                    for(int k = 0; k < C; k++){
                        max_pool_out << *(out_data_7 + i*R*C + j*C + k) << " ";
                    }max_pool_out << endl;
                }max_pool_out << endl;
                for (int j = 0; j < R; j++) {
                    for(int k = 0; k < C; k++){
                        max_pool_out << *(out_data_8 + i*R*C + j*C + k) << " ";
                    }max_pool_out << endl;
                }max_pool_out << endl;
            }max_pool_out.close();*/
#endif
#endif
    }
};
#endif
