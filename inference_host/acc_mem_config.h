
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

//#define HELLO_WORLD_REG_ADDR UINT64_C(0x0)

#define ACC_0_CTRL_PORT UINT64_C(0x010000)
#define ACC_1_CTRL_PORT UINT64_C(0x020000)
#define ACC_2_CTRL_PORT UINT64_C(0x030000)



#define HELLO_WORLD_REG_ADDR_CONTROL UINT64_C(0x00)
#define HELLO_WORLD_REG_ADDR_STATUS UINT64_C(0x04)

#define HELLO_WORLD_REG_ADDR_SRC_MSB_ADDR UINT64_C(0x1C)
#define HELLO_WORLD_REG_ADDR_DST_MSB_ADDR UINT64_C(0x24)

#define HELLO_WORLD_REG_ADDR_SRC_ADDR UINT64_C(0x18)
#define HELLO_WORLD_REG_ADDR_DST_ADDR UINT64_C(0x20)
#define HELLO_WORLD_REG_ADDR_BYTES UINT64_C(0x28)

// M_AXI_BAR1 connected to inference control port
//#define XINFERENCE_IP_CRTL_BUS_ADDR_1 UINT64_C(0x040000)
//#define XINFERENCE_IP_CRTL_BUS_ADDR_2 UINT64_C(0x020000)

//#define XINFERENCE_NET_CRTL_BUS_ADDR_AP_CTRL UINT64_C(0x0)
//#define XINFERENCE_NET_CRTL_BUS_ADDR_GIE UINT64_C(0x4)
//#define XINFERENCE_NET_CRTL_BUS_ADDR_IER UINT64_C(0x8)
//#define XINFERENCE_NET_CRTL_BUS_ADDR_ISR UINT64_C(0xc)

/* pci_bar_handle_t is a handler for an address space exposed by one PCI BAR on one of the PCI PFs of the FPGA */

pci_bar_handle_t pci_bar_handle = PCI_BAR_HANDLE_INIT;
pci_bar_handle_t pci_bar_handle_0 = PCI_BAR_HANDLE_INIT;
pci_bar_handle_t pci_bar_handle_4 = PCI_BAR_HANDLE_INIT;
pci_bar_handle_t pci_bar_handle_sda = PCI_BAR_HANDLE_INIT;

#define DDR_A UINT64_C(0x0000000C00000000)
#define DDR_B UINT64_C(0x0000000D00000000)
#define DDR_C UINT64_C(0x0000000E00000000)
#define DDR_D UINT64_C(0x0000000F00000000)

// output data buffer
#define O_BANK_0_0 UINT64_C(0xC0000000)
#define O_BANK_0_1 UINT64_C(0xC1000000)
#define O_BANK_0_2 UINT64_C(0xC2000000)
#define O_BANK_0_3 UINT64_C(0xC3000000)
#define O_BANK_0_4 UINT64_C(0xC4000000)
#define O_BANK_0_5 UINT64_C(0xC5000000)
#define O_BANK_0_6 UINT64_C(0xC6000000)
#define O_BANK_0_7 UINT64_C(0xC7000000)




#endif

