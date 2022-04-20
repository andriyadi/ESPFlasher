/* ESPFlasher C++ example

   Shows how to use DxESPFlasher component
*/

#include <cstdlib>
#include <thread>
#include "esp_log.h"
#include "driver/gpio.h"
#include "DxESPFlasher.h"

namespace utils = dx::utils;

#define HIGHER_BAUDRATE 230400

// Example of binary definitions
#define BOOTLOADER_ADDRESS  0x1000
#define PARTITION_ADDRESS   0x8000
#define APPLICATION_ADDRESS 0x10000

extern const uint8_t  ESP32_bootloader_bin[];
extern const uint32_t ESP32_bootloader_bin_size;
extern const uint8_t  ESP32_hello_world_bin[];
extern const uint32_t ESP32_hello_world_bin_size;
extern const uint8_t  ESP32_partition_table_bin[];
extern const uint32_t ESP32_partition_table_bin_size;

/**
 * @brief
 * Load example firmware which consists of 3 parts: bootloader, partition, and the app itself.
 * The actual bins are in `binaries/Hello-world/ESP32
 * Check `main/bin2array.cmake` to load another firmware.
 *
 * @param bins - output binary
 */
static void loadHelloworldBinary(utils::DxESPFirmwareBinary_t &bins) {
    bins.boot.data = ESP32_bootloader_bin;
    bins.boot.size = ESP32_bootloader_bin_size;
    bins.boot.address = BOOTLOADER_ADDRESS;

    bins.partition.data = ESP32_partition_table_bin;
    bins.partition.size = ESP32_partition_table_bin_size;
    bins.partition.address = PARTITION_ADDRESS;

    bins.app.data  = ESP32_hello_world_bin;
    bins.app.size  = ESP32_hello_world_bin_size;
    bins.app.address  = APPLICATION_ADDRESS;
}

extern "C" void app_main(void)
{
    // Declare firmware binary object to flash
    utils::DxESPFirmwareBinary_t binary = {};
    // Load it
    loadHelloworldBinary(binary);

    // Declare flasher config
    utils::DxESPFlasherConfig_t config = {115200,
                                          GPIO_NUM_5,
                                          GPIO_NUM_4,
                                          GPIO_NUM_25,
                                          GPIO_NUM_26};

    // Instantiate flasher object
    utils::DxESPFlasher flasher;

    // Begin. Will reboot if it fails.
    ESP_ERROR_CHECK(flasher.begin(config));

    // Connect to target MCU
    if (flasher.connectToTargetMCU(HIGHER_BAUDRATE) != ESP_LOADER_SUCCESS) {
        ESP_LOGE("APP", "Failed to connect to target MCU");
        return;
    }

    // Actually flash the firmware
    ESP_ERROR_CHECK(flasher.flashFirmware(binary));

    // Only shown if all is good
    ESP_LOGI("APP", "All's good");
}
