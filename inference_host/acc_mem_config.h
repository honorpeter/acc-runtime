
#ifndef _ACC_MEM_CONFIG_H_
#define _ACC_MEM_CONFIG_H_


#define ACC_NUM 3
//char acc_type[ACC_NUM] = {'CONV', 'CONV', 'FC'};

typedef float data_type;
typedef float data_type_w;
typedef float data_type_o;
// C++ compilation debug mode
#define _LAYER_MODE_ 1

// C++ compilation debug mode
#ifdef _LAYER_MODE_
#define _ACC_MODE_ 0
#else
#define _ACC_MODE_ 1
#endif

//#define _HLS_MODE_  1

//#define _BATCH_MODE_ 1
#ifdef _BATCH_MODE_
#define _KERNEL_DEBUG_ 0
#else
#define _KERNEL_DEBUG_ 1
#endif

#ifdef _HLS_MODE_
#define _C_DEBUG_MODE_ 0
#else
#define _C_DEBUG_MODE_ 1
#endif

// Accelerator buffer size configuration
const int IBUF_t = 32;
const int IBUF_d = 32;
const int WBUF_t = 32;
const int OBUF_t = 32;
const int Tm = 32;
const int Tn = 8;
const int Tr = 16;
const int Tc = 16;
const int S_max = 5;
const int K_max = 11;

//network configuration PARAMETERS
int weight_bias_record = 0;
int weight_bias_count_1 = 0;
int weight_bias_count_2 = 0;


int nn_in_data_size_conv[2] = {28, 14};
int nn_in_number_conv[2] = {1, 6};
int nn_out_number_conv[2] = {6, 16};
int nn_channel_size_conv[2] = {5, 5};
int nn_padding_conv[2] = {2, 0};
int nn_group_conv[2] = {1, 1};
int nn_in_data_size_pooling[2] = {28, 10};
int nn_in_number_pooling[2] = {6, 16};
int nn_channel_size_pooling[2] = {2, 2};
int nn_in_data_size_fc[1] = {5};
int nn_in_number_fc[1] = {16};
int nn_out_number_fc[1] = {10};
int nn_channel_size_fc[1] = {5};


static uint16_t pci_vendor_id = 0x1D0F; /* Amazon PCI Vendor ID */
static uint16_t pci_device_id = 0xF000; /* PCI Device ID preassigned by Amazon for F1 applications */

#define HELLO_WORLD_REG_ADDR UINT64_C(0x0)

#define ACC_0_CTRL_PORT UINT64_C(0x010000)
#define ACC_1_CTRL_PORT UINT64_C(0x020000)
#define ACC_2_CTRL_PORT UINT64_C(0x030000)

#define DDR_SH_ADDR UINT64_C(0xE01000000)
#define DDR_B_ADDR UINT64_C(0xD02000000)
#define DDR_A_ADDR UINT64_C(0xC02000000)

#define HELLO_WORLD_REG_ADDR_CONTROL UINT64_C(0x00)
#define HELLO_WORLD_REG_ADDR_STATUS UINT64_C(0x04)

#define HELLO_WORLD_REG_ADDR_SRC_MSB_ADDR UINT64_C(0x1C)
#define HELLO_WORLD_REG_ADDR_DST_MSB_ADDR UINT64_C(0x24)

#define HELLO_WORLD_REG_ADDR_SRC_ADDR UINT64_C(0x18)
#define HELLO_WORLD_REG_ADDR_DST_ADDR UINT64_C(0x20)
#define HELLO_WORLD_REG_ADDR_BYTES UINT64_C(0x28)

// M_AXI_BAR1 connected to inference control port
#define XINFERENCE_IP_CRTL_BUS_ADDR_1 UINT64_C(0x040000)
#define XINFERENCE_IP_CRTL_BUS_ADDR_2 UINT64_C(0x020000)

#define XINFERENCE_NET_CRTL_BUS_ADDR_AP_CTRL UINT64_C(0x0)
#define XINFERENCE_NET_CRTL_BUS_ADDR_GIE UINT64_C(0x4)
#define XINFERENCE_NET_CRTL_BUS_ADDR_IER UINT64_C(0x8)
#define XINFERENCE_NET_CRTL_BUS_ADDR_ISR UINT64_C(0xc)

/* pci_bar_handle_t is a handler for an address space exposed by one PCI BAR on one of the PCI PFs of the FPGA */

pci_bar_handle_t pci_bar_handle = PCI_BAR_HANDLE_INIT;
pci_bar_handle_t pci_bar_handle_0 = PCI_BAR_HANDLE_INIT;
pci_bar_handle_t pci_bar_handle_4 = PCI_BAR_HANDLE_INIT;
pci_bar_handle_t pci_bar_handle_sda = PCI_BAR_HANDLE_INIT;

// output data buffer
#define O_BUF_0 UINT64_C(0x04000000)
#define BUF_IN_0_1 UINT64_C(0x04001000)
#define BUF_IN_0_2 UINT64_C(0x04002000)
#define BUF_IN_0_3 UINT64_C(0x04003000)
#define BUF_IN_0_4 UINT64_C(0x04004000)
#define BUF_IN_0_5 UINT64_C(0x04005000)
#define BUF_IN_0_6 UINT64_C(0x04006000)
#define BUF_IN_0_7 UINT64_C(0x04007000)
#define BUF_IN_1_0 UINT64_C(0x04008000)
#define BUF_IN_1_1 UINT64_C(0x04009000)
#define BUF_IN_1_2 UINT64_C(0x0400A000)
#define BUF_IN_1_3 UINT64_C(0x0400B000)
#define BUF_IN_1_4 UINT64_C(0x0400C000)
#define BUF_IN_1_5 UINT64_C(0x0400D000)
#define BUF_IN_1_6 UINT64_C(0x0400E000)
#define BUF_IN_1_7 UINT64_C(0x0400F000)

//input weight buffer
#define CONV_W_BRAM_PCIS_0_0 UINT64_C(0x06000000)
#define CONV_W_BRAM_PCIS_0_1 UINT64_C(0x06001000)
#define CONV_W_BRAM_PCIS_0_2 UINT64_C(0x06002000)
#define CONV_W_BRAM_PCIS_0_3 UINT64_C(0x06003000)
#define CONV_W_BRAM_PCIS_0_4 UINT64_C(0x06004000)
#define CONV_W_BRAM_PCIS_0_5 UINT64_C(0x06005000)
#define CONV_W_BRAM_PCIS_0_6 UINT64_C(0x06006000)
#define CONV_W_BRAM_PCIS_0_7 UINT64_C(0x06007000)
/*
/ conv bias port --axi_bram_ctrl_20{0xCA00_0000,0xCA00_1000) (4k)
#define CONV_B_BRAM_PCIS UINT64_C(0x00030000)

// temp out 1 1 port --axi_bram_ctrl_11->18 [0xC600_0000, 0xC600_8000) 8 bram controller (4k/per)
#define BUF_OUT_1_0 UINT64_C(0x0A000000)
#define BUF_OUT_1_1 UINT64_C(0x0A001000)
#define BUF_OUT_1_2 UINT64_C(0x0A002000)
#define BUF_OUT_1_3 UINT64_C(0x0A003000)
#define BUF_OUT_1_4 UINT64_C(0x0A004000)
#define BUF_OUT_1_5 UINT64_C(0x0A005000)
#define BUF_OUT_1_6 UINT64_C(0x0A006000)
#define BUF_OUT_1_7 UINT64_C(0x0A007000)
#define BUF_OUT_2_0 UINT64_C(0x0A008000)
#define BUF_OUT_2_1 UINT64_C(0x0A009000)
#define BUF_OUT_2_2 UINT64_C(0x0A00A000)
#define BUF_OUT_2_3 UINT64_C(0x0A00B000)
#define BUF_OUT_2_4 UINT64_C(0x0A00C000)
#define BUF_OUT_2_5 UINT64_C(0x0A00D000)
#define BUF_OUT_2_6 UINT64_C(0x0A00E000)
#define BUF_OUT_2_7 UINT64_C(0x0A00F000)

// conv_param_in_port -- axi_bram_ctrl_1 (4k)
#define CONV_CORE_PARAM UINT64_C(0x00010000)
// pool_param_in_port_a -- axi_bram_ctrl_2 (4k)
#define POOL_CORE_PARAM UINT64_C(0x00020000)

#endif

