//
// Created by Andri Yadi on 20/04/22.
//

#ifndef DX_ESP_FLASHER_DXESPFIRMWAREBINARY_H
#define DX_ESP_FLASHER_DXESPFIRMWAREBINARY_H

#include <stdio.h>
#include <string.h>
#include <sys/param.h>

namespace dx {
    namespace utils {
        struct DxESPPartition_t {
            const uint8_t *data;
            uint32_t size;
            uint32_t address;
        };

        struct DxESPFirmwareBinary_t {
            DxESPPartition_t boot;
            DxESPPartition_t partition;
            DxESPPartition_t app;
        };
    }
}

#endif //DX_ESP_FLASHER_DXESPFIRMWAREBINARY_H
