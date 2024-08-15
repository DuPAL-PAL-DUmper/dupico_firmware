#include "command_hub_task.h"

#include "pico/stdlib.h"
#include "hardware/watchdog.h"

#include <task.h>
#include <stdint.h>

#include <common_macros.h>

#include <sys/sys.h>

#include <utils/custom_debug.h>

#include <tasks/interface_tasks/cli_interface_task.h>
#include <tasks/shifter_io_task.h>
#include <tasks/led_status_task.h>

typedef enum {
    READY,
    ERROR
} command_hub_status;

static void handle_inbound_commands(const command_hub_cmd *cmd, const QueueHandle_t resp_queue, const shifter_io_task_params* shifter_params, const led_status_task_params* lstatus_params, command_hub_status *hub_status);
static void handle_inbound_commands_simple_response(const command_hub_cmd *cmd, const QueueHandle_t resp_queue, command_hub_cmd_response_type resp, uint64_t data);
static bool reset_task(const shifter_io_task_params* shifter_params, const led_status_task_params* lstatus_params);
static void toggle_relay(bool state);

static bool reset_task(const shifter_io_task_params* shifter_params, const led_status_task_params* lstatus_params) {
    uint64_t shft_data;

    // Reset the status of the SIPO/PISO/Socket VCC
    toggle_relay(false); // Cut power to the socket

    xQueueSend(shifter_params->cmd_queue, (void*)& ((shifter_io_task_cmd){
        .cmd = SHF_WRITE,
        .param = 0
    }), portMAX_DELAY);

    xQueueSend(lstatus_params->cmd_queue, (void*)& ((led_status_task_cmd){
        .type = CMD_LSTAT_WAITING
    }), portMAX_DELAY);

    // Wait for the response
    return xQueueReceive(shifter_params->resp_queue, (void*)&(shft_data), portMAX_DELAY) == pdTRUE;
}

static void toggle_relay(bool state) {
    gpio_put(RELAY_ENABLE_GPIO, !state);
    vTaskDelay(50);
}

static void handle_inbound_commands(const command_hub_cmd *cmd, const QueueHandle_t resp_queue, const shifter_io_task_params* shifter_params, const led_status_task_params* lstatus_params, command_hub_status *hub_status) {
    uint64_t shft_data;

    switch(cmd->type) {
        case CMDH_RESET:
            *hub_status = reset_task(shifter_params, lstatus_params) ? READY : ERROR;
            handle_inbound_commands_simple_response(cmd, resp_queue, *hub_status == READY ? CMDH_RESP_OK : CMDH_RESP_ERROR, 0);
            break;
        case CMDH_FORCE_ERROR:
            toggle_relay(false); // Cut power to the socket
            *hub_status = ERROR;
            handle_inbound_commands_simple_response(cmd, resp_queue, CMDH_RESP_OK, 0); // Command recognized, even if it is intended to create an artificial error
            break;
        case CMDH_READ_PINS:
            DD_PRINTF("Got a READ request\r\n");

            xQueueSend(shifter_params->cmd_queue, (void*)& ((shifter_io_task_cmd){
                .cmd = SHF_READ,
                .param = 0
            }), portMAX_DELAY);

            if(xQueueReceive(shifter_params->resp_queue, (void*)&(shft_data), portMAX_DELAY) == pdTRUE) {
                handle_inbound_commands_simple_response(cmd, resp_queue, CMDH_RESP_OK, shft_data);
            } else {
                handle_inbound_commands_simple_response(cmd, resp_queue, CMDH_RESP_ERROR, 0);
                D_PRINTF("Error handling a READ request!\r\n");
                *hub_status = ERROR;
            }
            break;
        case CMDH_OSC_DET: {
                DD_PRINTF("Got a OSC_DET request, count %u\r\n", (cmd->data & 0xFF));
                uint64_t flipped_pins = 0;
                uint64_t prev_shft_data = 0;

                for(uint16_t idx = 0; idx < (cmd->data & 0xFF); idx++) {
                    xQueueSend(shifter_params->cmd_queue, (void*)& ((shifter_io_task_cmd){
                        .cmd = SHF_READ,
                        .param = 0
                    }), portMAX_DELAY);

                    if(xQueueReceive(shifter_params->resp_queue, (void*)&(shft_data), portMAX_DELAY) == pdTRUE) {
                        if(idx > 0) {
                            flipped_pins |= shft_data ^ prev_shft_data;
                        } else {
                            prev_shft_data = shft_data;
                        }
                    } else {
                        handle_inbound_commands_simple_response(cmd, resp_queue, CMDH_RESP_ERROR, 0);
                        D_PRINTF("Error handling a OSC_DET request!\r\n");
                        *hub_status = ERROR;
                        break;
                    }  
                }

                if(*hub_status != ERROR) handle_inbound_commands_simple_response(cmd, resp_queue, CMDH_RESP_OK, flipped_pins);
            }
            break;
        case CMDH_WRITE_PINS:
            DD_PRINTF("Got a WRITE request %llx\r\n", cmd->data);

            xQueueSend(shifter_params->cmd_queue, (void*)& ((shifter_io_task_cmd){
                .cmd = SHF_WRITE,
                .param = cmd->data
            }), portMAX_DELAY);

            if(xQueueReceive(shifter_params->resp_queue, (void*)&(shft_data), portMAX_DELAY) == pdTRUE) {
                handle_inbound_commands_simple_response(cmd, resp_queue, CMDH_RESP_OK, shft_data);
            } else {
                handle_inbound_commands_simple_response(cmd, resp_queue, CMDH_RESP_ERROR, 0);
                D_PRINTF("Error handling a WRITE request!\r\n");
                *hub_status = ERROR;
            }
            break;
        case CMDH_TOGGLE_POWER:
            DD_PRINTF("Got a relay toggle command %u\r\n", cmd->data);
            toggle_relay(cmd->data > 0);
            handle_inbound_commands_simple_response(cmd, resp_queue, CMDH_RESP_OK, cmd->data > 0 ? 1 : 0);
            break;
        default:
            handle_inbound_commands_simple_response(cmd, resp_queue, CMDH_RESP_ERROR, 0);
            break;
    }
}

static void handle_inbound_commands_simple_response(const command_hub_cmd *cmd, const QueueHandle_t resp_queue, command_hub_cmd_response_type resp, uint64_t data) {
    xQueueSend(resp_queue, (void*)& ((command_hub_cmd_resp){
        .cmd_type = cmd->type,
        .type = resp,
        .data = (command_hub_cmd_resp_data) {
            .data = data
        }
    }), portMAX_DELAY);
}

void command_hub_task(void *params) {
    TaskHandle_t cli_interface_t_handle, shifter_io_t_handle, lstatus_t_handle;
    
    command_hub_cmd cmd;

    // Queues to send the updates to the CLI task
    // Queues to handle reception of commands and responses from CLI task
    command_hub_queues cli_queues = {
        .cmd_queue = xQueueCreate(CMD_QUEUE_SIZE, sizeof(command_hub_cmd)),
        .resp_queue = xQueueCreate(CMD_QUEUE_SIZE + 1, sizeof(command_hub_cmd_resp)) // We can hold more responses than commands, should avoid blocking as we have just one producer/consumer
    };

    shifter_io_task_params shifter_params = {
        .piso_cfg = {
            .ce_pin = PISO_INH_GPIO,
            .pe_pin = PISO_SH_GPIO,
            .clk_pin = PISO_CLK_GPIO,
            .clr_pin = PISO_CLR_GPIO,
            .ser_pin = PISO_SER_GPIO,
            .len = 40
        },
        .sipo_cfg = {
            .oe_pin = SIPO_OE_GPIO,
            .rclk_pin = SIPO_RCLK_GPIO,
            .ser_pin = SIPO_SER_GPIO,
            .srclk_pin = SIPO_CLK_GPIO,
            .srclr_pin = SIPO_CLR_GPIO,
            .len = 40
        },
        .cmd_queue = xQueueCreate(CMD_QUEUE_SIZE, sizeof(shifter_io_task_cmd)),
        .resp_queue = xQueueCreate(CMD_QUEUE_SIZE + 1, sizeof(uint64_t))
    };

    led_status_task_params lstatus_params = {
        .cmd_queue = xQueueCreate(2, sizeof(led_status_task_cmd)),
        .led_gpio = LED_GPIO
    };

    // Create and start the task to handle SIPO/PISO interfacing
    xTaskCreate(shifter_io_task, "ShifterIOTask", configMINIMAL_STACK_SIZE, (void*)&shifter_params, BASELINE_TASK_PRIORITY, &shifter_io_t_handle);

    // Create and start the task to handle CLI interface
    xTaskCreate(cli_interface_task, "CLIInterfaceTask", configMINIMAL_STACK_SIZE * 2, (void*)&cli_queues, BASELINE_TASK_PRIORITY, &cli_interface_t_handle);

    // Create and start the task to handle the status led
    xTaskCreate(led_status_task, "LEDStatusTask", configMINIMAL_STACK_SIZE, (void*)&lstatus_params, BASELINE_TASK_PRIORITY, &lstatus_t_handle);

    command_hub_status status = reset_task(&shifter_params, &lstatus_params) ? READY : ERROR;

    // Variables used to check if we need to update the led status or not
    led_status_cmd_type last_led_status = CMD_LSTAT_WAITING;
    led_status_cmd_type cur_led_status = CMD_LSTAT_WAITING;
    bool serial_connected = false;

    while(status != ERROR) {
        // Receive commands from the CLI
        while(xQueueReceive(cli_queues.cmd_queue, (void*)&(cmd), 0) && status != ERROR) {
            handle_inbound_commands(&cmd, cli_queues.resp_queue, &shifter_params, &lstatus_params, &status);

            // Update the status led every time we have handled a command,
            // If we're in a ready state and connected to a terminal, then we request a "CONNECTED" state to be shown,
            // if we're ready but not connected, we'll request a "WAITING". Otherwise it's an "ERROR".

            serial_connected = stdio_usb_connected();
            cur_led_status = (status == READY) ? (serial_connected ? CMD_LSTAT_CONNECTED : CMD_LSTAT_WAITING) : CMD_LSTAT_ERROR;
            if (cur_led_status != last_led_status) { // Yep, we need an update!
                last_led_status = cur_led_status;
                xQueueSend(lstatus_params.cmd_queue, (void*)& ((led_status_task_cmd){
                    .type = (status == READY) ? (serial_connected ? CMD_LSTAT_CONNECTED : CMD_LSTAT_WAITING) : CMD_LSTAT_ERROR
                }), portMAX_DELAY);
            }
            
            taskYIELD();         
        }

        taskYIELD();
        watchdog_update();
    }

    D_PRINTF("WARNING: Out of command hub loop, state %d\r\n", status);
    vTaskDelete(NULL);
}