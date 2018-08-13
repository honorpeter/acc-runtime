//------------- Rights to yao.chen@adsc-create.edu.sg-------------------//
// Created by yaochen on 9/4/18.
//
// This file is used to test the port status of the accelerators 
// Including: 1) acc ctrl port initial status read and print
//            2) acc computation start and stop status check and verify
//
//------------- Rights to yao.chen@adsc-create.edu.sg-------------------//

// TODO: simplify header files
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <fpga_pci.h>
#include <fpga_mgmt.h>

#include "../inference_host/inference_func.h"
#include "../inference_host/acc_mem_config.h"

int acc_ctrl_test(XInference_net Ptr, int slot_id, int pf_id, int bar_id) {
    
    int status;
    int loop_var = 0;

    uint64_t test_addr[3] = { ACC_0_CTRL_PORT, ACC_1_CTRL_PORT, ACC_2_CTRL_PORT };
    int rc_4;

    static const size_t buffer_size = 16;

    /* pci_bar_handle_t is a handler for an address space exposed by one PCI BAR on one of the PCI PFs of the FPGA */
    pci_bar_handle_t pci_bar_handle_4 = PCI_BAR_HANDLE_INIT;

    /* attach to the fpga, with a pci_bar_handle out param
     * To attach to multiple slots or BARs, call this function multiple times,
     * saving the pci_bar_handle to specify which address space to interact with in
     * other API calls.
     * This function accepts the slot_id, physical function, and bar number
     */
    rc_4 = fpga_pci_attach(slot_id, pf_id, 4, 0, &pci_bar_handle_4);
    fail_on(rc_4, out, "Unable to attach to the AFI on slot id %d", slot_id);

    /*Read the accelerator control port register status and show in terminal*/
    uint32_t port_status[3];
    port_status[0] = XInference_net_ReadReg(pci_bar_handle_4, test_addr[0], 0);
    port_status[1] = XInference_net_ReadReg(pci_bar_handle_4, test_addr[1], 0);
    port_status[2] = XInference_net_ReadReg(pci_bar_handle_4, test_addr[2], 0);
    
    for (uint32_t i = 0; i < 3; i++) {
	   cout << "Returned acc_" <<i<<" ctrl_port status = " << port_status[i] << endl;
       if (port_status[i] == 0x00) {cout << "acc" << i << "port check SUCCESS!" << endl;}
       else {cout << "acc" << i << "port check Failure!" << endl;}
    }

    uint32_t acc_status[3];
    for (uint32_t i = 0; i < 3; i++) {
        XInference_net_Start(pci_bar_handle_4, Ptr);
        while (!XInference_net_IsDone(pci_bar_handle_4, Ptr)) { ; }
//        cout << "Acc_[" << i << "] Idling Test SUCCESS!!" << endl;
        cout << "Acc_0 test time " << i << "  --> SUCCESS !" << endl;
    }

out:
    /* clean up */
    if (pci_bar_handle_4 >= 0) {
        rc_4 = fpga_pci_detach(pci_bar_handle_4);
        if (rc_4) {
            printf("Failure while detaching from the fpga.\n");
        }
    }
    /* if there is an error code, exit with status 1 */
    return (rc_4 != 0 ? 1 : 0);
}

// acc_x_ctrl_port test main function
int main(int argc, char** argv) {
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

    printf("=====Accelerators CTRL PORT TEST =====\n");   

    XInference_net Acc_ptr_0;

    Acc_ptr_0.ctrl_bus_baseaddress = ACC_0_CTRL_PORT;
    Acc_ptr_0.IsReady = 0x01;
//    Acc_ptr[1].ctrl_bus_baseaddress = ACC_1_CTRL_PORT;
//    Acc_ptr[1].IsReady = 0x01;
//    Acc_ptr[2].ctrl_bus_baseaddress = ACC_2_CTRL_PORT;
//    Acc_ptr[2].IsReady = 0x01;

    rc = acc_ctrl_test(Acc_ptr_0, slot_id, FPGA_APP_PF, APP_PF_BAR1);
    fail_on(rc, out, "CTRL TEST failed");
  
    return rc; 
   
out:
    return 1;
}
