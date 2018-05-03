//
// Created by yaochen on 9/4/18.
//

/*TODO: Add the header files here*/
// Note: only add the required files here
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <fpga_pci.h>
#include <fpga_mgmt.h>

#include "../inference_host/inference_func.h"
#include "../inference_host/acc_mem_config.h"

/* use the stdout logger */
const struct logger *logger = &logger_stdout;

/* TODO: complete all this functions and pack them as unitTest APIs */
// weight/input/output memory test function
//int acc_mem_test(uint64_t startAddress, uint64_t stepLength, uint64_t endAddress);
// control & parameter input register memory test function
//int acc_mem_test(uint64_t startAddress, uint64_t stepLength);

int acc_mem_test(int rc, int fd, uint64_t BRAM_ADDRESS, float *write_data, float *read_data,
                 int buffer_size, int count) {
    
    int error_num = 0;
    int loop_var = 0;
    int flag = 1;
    int channel = 0;
    size_t write_offset_all = 0;
    size_t read_offset_all = 0;

    //error_num = set_pwrite_pread(rc, fd, write_data, read_data, buffer_size, BRAM_ADDRESS, count);
    
    while ( channel < count ) {
        size_t write_offset = 0;
        while (write_offset < buffer_size * 4) {
            rc = pwrite(fd,
                write_data + write_offset / 4,
                buffer_size * 2,
                BRAM_ADDRESS + write_offset_all + write_offset);
            if (rc < 0) {
                fail_on((rc = (rc < 0)? errno:0), out, "call to pwrite failed.");
            }
            write_offset += rc;
            write_offset_all += rc;
        }
        rc = 0;

        rc = fsync(fd);
        fail_on((rc = (rc < 0)? errno:0), out, "call to fsync failed.");

        size_t read_offset = 0;
        while (read_offset < buffer_size * 4) {
            rc = pread(fd,
                read_data + read_offset / 4,
                buffer_size * 2,
                BRAM_ADDRESS + read_offset_all + read_offset);
            if (rc < 0) {
                fail_on((rc = (rc < 0)? errno:0), out, "call to pread failed.");
            }
            read_offset += rc;
            read_offset_all += rc;
        }
        rc = 0;

        //compare write_data & read_data
        while (loop_var < buffer_size) {
            if (write_data[loop_var] == read_data[loop_var]) {
                flag = 1;
            } else {
                flag = 0;
                cout << "write and read data comparison error at " << loop_var << endl;
                break;
            }
            loop_var++;
        }

        loop_var = 0;

        printf("Bytes written:\n");
        for (int i = 0; i < buffer_size; ++i) {
            cout << write_data[i] << " ";
        }

        printf("\n\n");

        printf("Bytes read:\n");
        for (int i = 0; i < buffer_size; ++i) {
            cout << read_data[i] << " ";
        }

        printf("\n\n");

        if (flag == 0){
            error_num = channel + 1;
            break;
        }

        channel ++ ;
    }

out:
    if (fd >= 0) {
        close(fd);
    }

    return error_num;
}

int example(int slot_id, int pf_id, int bar_id) {
    
    int error_num;
    int addr_num = 0;

    uint64_t start_addr = CONV_W_BRAM_PCIS_0_0;
    uint64_t block_size = 0x00001000;
    uint64_t end_addr = CONV_W_BRAM_PCIS_0_5;
    uint64_t tmp_addr = start_addr;

    int rc_4;

    int fd, rc1;
    char device_file_name[256];
    float *write_buffer, *read_buffer;
    static const size_t buffer_size = 1024;
    read_buffer = NULL;
    write_buffer = NULL;
    fd = -1;

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

    rc1 = log_init("test_dram_dma");
    //fail_on(rc1, out, "Unable to initialize the log.");
    rc1 = log_attach(logger, NULL, 0);
    fail_on(rc1, out, "%s", "Unable to attach to the log.");

    /* initialize the fpga_plat library */
    rc1 = fpga_mgmt_init();
    fail_on(rc1, out, "Unable to initialize the fpga_mgmt library");

    rc1 = sprintf(device_file_name, "/dev/edma%i_queue_0", slot_id);
    fail_on((rc1 = (rc1 < 0) ? 1 : 0), out, "Unable to format device file name.");
    printf("device_file_name=%s\n", device_file_name);

    /* make sure the AFI is loaded and ready */
    rc1 = check_slot_config(slot_id);
    fail_on(rc1, out, "slot config is not correct");

    fd = open(device_file_name, O_RDWR);

    if (fd < 0) {
        printf("Cannot open device file %s.\nMaybe the EDMA "
                       "driver isn't installed, isn't modified to attach to the PCI ID of "
                       "your CL, or you're using a device file that doesn't exist?\n"
                       "See the edma_install manual at <aws-fpga>/sdk/linux_kernel_drivers/edma/edma_install.md\n"
                       "Remember that rescanning your FPGA can change the device file.\n"
                       "To remove and re-add your edma driver and reset the device file mappings, run\n"
                       "`sudo rmmod edma-drv && sudo insmod <aws-fpga>/sdk/linux_kernel_drivers/edma/edma-drv.ko`\n",
               device_file_name);
        fail_on((rc1 = (fd < 0) ? 1 : 0), out, "unable to open DMA queue. ");
    }

    write_buffer = (float *) malloc (buffer_size * sizeof(float));
    read_buffer = (float *) malloc (buffer_size * sizeof(float));

    if (write_buffer == NULL || read_buffer == NULL) {
        rc1 = ENOMEM;
        goto out;
    }

    for (int i = 0; i < buffer_size; i++) {
        write_buffer[i] = 1.1;
    }

    while (tmp_addr <= end_addr) {
        tmp_addr += block_size;
        addr_num++;
    }

    error_num = acc_mem_test(rc1, fd, start_addr, write_buffer, read_buffer, buffer_size, addr_num);

    if (error_num == 0) {
        cout << "test " << addr_num << " addresses" << endl;
        cout << "read the same string as it write!" << endl;
    } else
        cout << "at " << error_num << "th mem address" << " read not the same string as it write!" << endl;

out:
    /* clean up */
    if (write_buffer != NULL) {
        free(write_buffer);
    }
    if (read_buffer != NULL) {
        free(read_buffer);
    }
    if (pci_bar_handle_4 >= 0) {
        rc_4 = fpga_pci_detach(pci_bar_handle_4);
        if (rc_4) {
            printf("Failure while detaching from the fpga.\n");
        }
    }
    /* if there is an error code, exit with status 1 */
    return (rc_4 != 0 ? 1 : 0);
}

// memory spaces test main function
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

    printf("===== MEM TEST =====\n");

    rc = example(slot_id, FPGA_APP_PF, APP_PF_BAR1);
    fail_on(rc, out, "MEM TEST failed");

    return rc;

out:
    return 1;
    // if different control statuses passed
    //return 0;
}
