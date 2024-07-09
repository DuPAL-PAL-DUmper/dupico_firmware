#include "cli_interface_task.h"

#include <task.h>

#include "utils/custom_debug.h"
#include "tusb.h"

#define VERSION "0.0.1"
#define SOFT_HEADER "\nDuPICO - " VERSION "\n\n\r"

void cli_interface_task(void *params) {
    bool term_connected_state = false;
    bool cur_term_connected = false;

    while(true) {
        cur_term_connected = tud_cdc_connected();

        if(cur_term_connected != term_connected_state) {
            term_connected_state = cur_term_connected;

            if(!term_connected_state) { // We got a disconnection from the device
                D_PRINTF("Serial terminal disconnected!\r\n");

                // TODO: Disable the relay, reset PISO/SIPO and every other state we need to reset
            } else { // New connection!
                D_PRINTF("Serial terminal connected!\r\n");

                USB_PRINTF(SOFT_HEADER);
                USB_PRINTF("REMOTE_CONTROL_ENABLED\r\n");
            }
        }

        taskYIELD();
    } 
}