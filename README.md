# ESPFlasher
A simple component and example to flash firmware to ESP32 from.... ESP32 :)

The project contains a component: [DxFlasher](https://github.com/andriyadi/ESPFlasher/tree/main/components/DxFlasher), that wraps [esp-serial-flasher](https://github.com/espressif/esp-serial-flasher) component due to reasons:
* To provide C++ component
* Make it easier to use and possibly extend to support more functionality in the future

## Why?
Why do on earth we need to flash ESP32 from anothe ESP32, you may ask?
* Able to flash ESP32 without using computer/laptop/SBC (e.g Raspberry Pi) should be beneficial and practical, especially in the field.
* Because we can

## How to use
Just look at [`main.cpp`](https://github.com/andriyadi/ESPFlasher/blob/main/main/main.cpp), should be self-descriptive.

Especially check function of `loadHelloworldBinary`. Notes:
* It loads example firmware which consists of 3 parts: bootloader, partition, and the app itself.
* The actual bins are in `binaries/Hello-world/ESP32
* Check `main/bin2array.cmake` to load another firmware.


## Credits
This project is not possible without this awesome component: [esp-serial-flasher](https://github.com/espressif/esp-serial-flasher)