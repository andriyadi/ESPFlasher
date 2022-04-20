//
// Created by Andri Yadi on 20/04/22.
//

#ifndef DX_ESP_FLASHER_DXESPFLASHER_H
#define DX_ESP_FLASHER_DXESPFLASHER_H

#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include "esp_err.h"
#include "serial_io.h"
#include "esp_loader.h"

extern "C" {
#include "esp32_port.h"
}

#include "DxESPFirmwareBinary.h"

namespace dx {
    namespace utils {

#define DX_FLASHER_WRITE_FLASH_BUFFER_SIZE (2048)//(1024)

        struct DxESPFlasherConfig_t: public loader_esp32_config_t {
            DxESPFlasherConfig_t(uint32_t baudRate, uint32_t rxPin, uint32_t txPin, uint32_t resetPin, uint32_t gpio0Pin);
        };

        class DxESPFlasher {
        public:
            explicit DxESPFlasher();
            ~DxESPFlasher();

            esp_err_t begin(const DxESPFlasherConfig_t &config);

            esp_loader_error_t connectToTargetMCU(uint32_t higher_baudrate);

            esp_loader_error_t flashFirmware(const DxESPFirmwareBinary_t &binary);
            esp_loader_error_t flashBinary(const uint8_t *bin, size_t size, size_t address);

        private:
            bool connected_ = false;
        };

    }
}

#endif //DX_ESP_FLASHER_DXESPFLASHER_H
