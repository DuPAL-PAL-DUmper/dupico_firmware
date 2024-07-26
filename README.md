# DuPAL V3 "dupico" firmware

## Introduction

The DuPAL (**Du**mper of **PAL**s) is a set of software and hardware instruments that I developed to help me bruteforcing and analyzing PLCs (and ROMs, and other things), with the objective of eventually being able to dump and save the ones in the circuit boards I have around.

**Note:** The current version of this firmware is compatible with DuPAL V3 "dupico" boards __only__.

## Sponsorship

![PCBWay_logo](pics/pcbway_logo.png)

This project is kindly sponsored by [PCBWay](https://pcbway.com).
PCBWay specializes in manufacturing high-quality PCBs and makes them affordable to hobbyist and professionals alike.

The range of services they offer include PCB prototyping, assembly, instant quotes for your order, a verification process by a team
of experts and an easy to use, hassle-free order process.

I'm grateful to PCBWay for the support in creating this project.

### Building

Building requires an ARM toolchain, cmake and both [FreeRTOS](https://github.com/FreeRTOS/FreeRTOS-Kernel.git) and the [Pico SDK](https://github.com/raspberrypi/pico-sdk.git) checked out, with their locations set respectively in the environment variables `FREERTOS_KERNEL_PATH` and `PICO_SDK_PATH`.

**TODO**: Add better build instructions.

### Connecting to the DuPAL V3

Connecting the USB connection of the Pico to the PCB will present a virtual serial port to which you can connect. Upon connection, you should get strings similar to the following, in your terminal:

```
DuPICO - 0.0.2

REMOTE_CONTROL_ENABLED
```

#### Remote Control mode

Entering **Remote Control** mode puts the board in a state where it waits commands from the host.

The commands are in ASCII format and can be input by hand, but the mode is meant to be leveraged by an external application that can pilot the board to perform advanced analisys.

##### Remote Control protocol

The **Remote Control** protocol is pretty simple and ASCII based. It supports few commands, each with its own syntax and response.

A string `CMD_ERR` will be sent in case the command is not recognized.

###### Write

- Syntax: `>W xxxxxxxxxxxxxxxx<`
- Response: `[W yyyyyyyyyyyyyyyy]`

Where `xxxxxxxxxxxxxxxx` is the hex representation of the status to apply to pins on the socket, starting at pin 1 for the LSB, ending at pin 41 at bit 39. The remaining pins are currently ignored and are reserved for future use.

Note that **pin 21 is skipped** as it is fixed at GND, so the mapping skips from pin 20 associated at bit 19 to pin 22 associated at bit 20.

`yyyyyyyyyyyyyyyy` is the value read from the pins, and follows the same mapping as used for the write.

###### Extended Write

- Syntax: `>W xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx<`
- Response: `[W yyyyyyyyyyyyyyyy yyyyyyyyyyyyyyyy yyyyyyyyyyyyyyyy yyyyyyyyyyyyyyyy yyyyyyyyyyyyyyyy yyyyyyyyyyyyyyyy yyyyyyyyyyyyyyyy yyyyyyyyyyyyyyyy]`

Performs 8 writes and reads in sequence, then provides the answer. Same mapping as for the simple write command.

###### Read

- Syntax: `>R<`
- Response: `[R xxxxxxxxxxxxxxxx]`

Where `xxxxxxxxxxxxxxxx` is the hex representation of the status of the pins. Follows the same mapping as used for the Write command and its response.

###### Power

- Syntax: `>P x<`
- Response: `[P x]`

Where `x` is either 0 or 1.

This command enables (1) or disables (0) power to the VCC pin (42) on the 42 pin socket.

The outputs of the shift registers are **not** enabled if the power is not applied to the socket.

###### Power

- Syntax: `>P x<`
- Response: `[P x]`

Where `x` is either 0 or 1.

This command enables (1) or disables (0) power to the VCC pin (42) on the 42 pin socket.

###### Reset

- Syntax: `>K<`
- Response: No response, the board will reset

This command forces a reset by watchdog of the dupico.

###### Get Model

- Syntax: `>M<`
- Response: `[M x]`

Where `x` is a code that identifies the model of the DuPAL. For dupico boards it is currently `3`.

###### Self-Test

- Syntax: `>T<`
- Response: `[T x]`

**Remove all ICs and adapter from the socket before executing the test!**

Where `x` is either 1 for a passing test or 0 for a failing one. This command executes a quick self-test of the shifters on the dupico.

## Hardware notes

### Recognizing three-state outputs

To recognize wether a pin is an input, an output, or an output in hi-z mode, the DuPAL uses the following method:

Every possible output is connected to the MCU (actually to the shift registers, but this is beside the point) by using two pins:

- One connection will be direct, and will be to an *input* of our microcontroller
- The second connection will be through a resistor of relatively high value (e.g. 10k) to an *output* pin of our microcontroller

```text
    IC                              MCU
             |       Resistor  |
Unknown PIN  |-----o--/\/\/\---| Output PIN
             |     |           |
             |     `-----------| Input PIN
```

The board will then do the following:

1. Set the **MCU output** as **high**
2. Read the **MCU input**
    - If it is **low**, then the **IC pin** is an **output** (because it's in a different state than what we're pulling it to be)
    - If it is **high**, go on with the test
3. Set the **MCU output** as **low**
4. Read the **MCU input**
    - If it is **high**, then the **IC pin** is an **output**
    - Otherwise, the **IC pin** is either an **input** or an output in **hi-Z** (see below)

Note that, in case the power is not applied to the socket (See the *Power* command above), the output pins of the registers connected to the socket will not be enabled, and all the pins will be pulled low with weak (470K) pulldowns.
