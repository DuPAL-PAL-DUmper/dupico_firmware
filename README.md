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

Building requires an ARM toolchain, [CMake](https://cmake.org/) and both [FreeRTOS](https://github.com/raspberrypi/FreeRTOS-kernel) and the [Pico SDK](https://github.com/raspberrypi/pico-sdk.git) checked out, with their locations set respectively in the environment variables `FREERTOS_KERNEL_PATH` and `PICO_SDK_PATH`.

**TODO**: Add better build instructions.

### Connecting to the DuPAL V3

Connecting the USB connection of the Pico to the PCB will present a virtual serial port to which you can connect. Upon connection, you should get strings similar to the following, in your terminal:

```
REMOTE_CONTROL_ENABLED
```

#### Control protocol

TODO: describe the binary protocol

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
