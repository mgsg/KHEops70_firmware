# KHEops70 QMK firmware
QMK Firmware for the KHEops70 keyboard

**K**eyboard | **H**all **E**ffect | **O**rthoLinear | **P**rogrammable | **S**tandard-sized

Check Hardware Repository [here](https://github.com/mgsg/KHEops70).

    WARNING: DIY project. Use at your own risk!


## Project Description

**KHEops70** is a **K**eyboard that uses **HE** (Hall Effect/analog/magnetic) sensors, has an **O**rthogonal layout, is **P**rogrammable (using QMK) and **S**tandard-sized and has **70** keys.

<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img title="KHEops70" alt="KHEops70" src="../KHEops70/blob/main/images/KHEops70.png" width="400"></p>


* It is a **MoonBoard** variant. [Check the MoonBoard project here.](https://github.com/certainly1182/MoonBoard) [Check the QMK firmware for the MoonBoard here.](https://github.com/RephlexZero/qmk_firmware/blob/adc_testing/keyboards/rephlex/moonboard/readme.md).
* Goal: Hybrid keyboard for office/gaming typing AND music experimenting!


## Features:
  - Hall Effect (HE) / Analog / Magnetic sensors with configurable actuation point for typing and MIDI velocity sensing.
  - Per-key RGB Leds.
  - USB connection uses [Unified Daughterboard (UD)](https://unified-daughterboard.github.io/).
  - Orthogonal layout.
  - Programmable using QMK Firmware (VIA, VIAL).
  - Standard size for many 60% cases (GH60, Wooting60 with slight modifications for the UD).
  - 70 keys.
  - Can be expanded to add 2 finger drum pads using piezo sensors.
  - Rotary encoder.
  - OLED display

* Keyboard Maintainer: [mgsg](https://github.com/mgsg)
* Hardware Supported: *STM32F303*
* Hardware Availability: *NULL*

## Build instructions

Clone the QMK repository and follow the configuration instructions for your platform. Copy the `mgsg/kheops70` folder from this repository inside your QMK project `keyboards` folder.

Make example for this keyboard (after setting up your build environment):

    make mgsg/kheops70:default

Flashing example for this keyboard (or QMK Toolbox for supported platforms):

    make mgsg/kheops70:default:flash

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and
the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start
with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the top left key and plug in the keyboard (Typically Esc).
* **Physical reset button**: Briefly hold the button on the back of the PCB whilst plugging in the keyboard.
* **Keycode in layout**: A key combination is mapped to `QK_BOOT` in the pre-created keymap: *Fn-B*. Press it for 2 seconds to enter DFU mode.
