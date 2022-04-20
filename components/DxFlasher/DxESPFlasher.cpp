//
// Created by Andri Yadi on 20/04/22.
//

#include "DxESPFlasher.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#define DXFLASHER_RETURN_ON_ERROR(x, log_tag, format, ...) do {                                \
        esp_loader_error_t err_rc_ = (x);                                                      \
        if (unlikely(err_rc_ != ESP_LOADER_SUCCESS)) {                                         \
            ESP_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);       \
            return err_rc_;                                                                    \
        }                                                                                      \
    } while(0)

namespace dx {
    namespace utils {

        constexpr auto *TAG = "FLASHER";

        DxESPFlasherConfig_t::DxESPFlasherConfig_t(uint32_t baudRate, uint32_t rxPin, uint32_t txPin, uint32_t resetPin,
                                                   uint32_t gpio0Pin) : loader_esp32_config_t() {
            baud_rate = baudRate;
            uart_rx_pin = rxPin;
            uart_tx_pin = txPin;
            reset_trigger_pin = resetPin;
            gpio0_trigger_pin = gpio0Pin;

            uart_port = UART_NUM_1;
        }



        DxESPFlasher::DxESPFlasher() {

        }

        DxESPFlasher::~DxESPFlasher() {

        }

        esp_err_t DxESPFlasher::begin(const DxESPFlasherConfig_t &config) {

            connected_ = false;

            if (loader_port_esp32_init(&config) != ESP_LOADER_SUCCESS) {
                ESP_LOGE(TAG, "serial initialization failed.");
                return ESP_FAIL;
            }

            return ESP_OK;
        }

        esp_loader_error_t DxESPFlasher::connectToTargetMCU(uint32_t higher_baudrate) {

            esp_loader_connect_args_t connect_config = ESP_LOADER_CONNECT_DEFAULT();

            esp_loader_error_t err = esp_loader_connect(&connect_config);
            if (err != ESP_LOADER_SUCCESS) {
                ESP_LOGE(TAG, "Cannot connect to target. Error: %u", err);
                return err;
            }

            ESP_LOGI(TAG, "Connected to target");

            if (higher_baudrate && esp_loader_get_target() != ESP8266_CHIP) {

                ESP_LOGI(TAG, "Changing baud rate to %d", higher_baudrate);

                err = esp_loader_change_baudrate(higher_baudrate);
                if (err == ESP_LOADER_ERROR_UNSUPPORTED_FUNC) {
                    ESP_LOGE(TAG, "Does not support change baudrate command.");
                    return err;
                } else if (err != ESP_LOADER_SUCCESS) {
                    ESP_LOGE(TAG, "Unable to change baud rate on target.");
                    return err;
                } else {
                    err = loader_port_change_baudrate(higher_baudrate);
                    if (err != ESP_LOADER_SUCCESS) {
                        ESP_LOGE(TAG, "Unable to change baud rate.");
                        return err;
                    }

                    ESP_LOGI(TAG, "Baud rate to changed\n");
                }
            }

            connected_ = true;
            return ESP_LOADER_SUCCESS;
        }

        esp_loader_error_t DxESPFlasher::flashFirmware(const DxESPFirmwareBinary_t &binary) {

            if (!connected_) {
                return ESP_LOADER_ERROR_INVALID_TARGET;
            }

            // Check binary validity here

            ESP_LOGI(TAG, "Flashing bootloader");
            DXFLASHER_RETURN_ON_ERROR(flashBinary(binary.boot.data, binary.boot.size, binary.boot.address), TAG, "");
            printf("\n");
            ESP_LOGI(TAG, "Flashing partition");
            DXFLASHER_RETURN_ON_ERROR(flashBinary(binary.partition.data, binary.partition.size, binary.partition.address), TAG, "");
            printf("\n");
            ESP_LOGI(TAG, "Flashing application");
            DXFLASHER_RETURN_ON_ERROR(flashBinary(binary.app.data, binary.app.size, binary.app.address), TAG, "");
            printf("\n");

            return ESP_LOADER_SUCCESS;
        }

        esp_loader_error_t DxESPFlasher::flashBinary(const uint8_t *bin, size_t size, size_t address) {

            if (!connected_) {
                return ESP_LOADER_ERROR_INVALID_TARGET;
            }

            esp_loader_error_t err;
            static uint8_t payload[DX_FLASHER_WRITE_FLASH_BUFFER_SIZE];
            const uint8_t *bin_addr = bin;

            //ESP_LOGI(TAG, "Erasing flash (this may take a while)...");
            ESP_LOGI(TAG, "Flash will be erased from 0x%X to 0x%X", address, (address + size));

            err = esp_loader_flash_start(address, size, sizeof(payload));
            if (err != ESP_LOADER_SUCCESS) {
                ESP_LOGE(TAG, "Erasing flash failed with error %d.", err);
                return err;
            }

            //ESP_LOGI(TAG, "Starting programming...");
            uint64_t _startProg = esp_timer_get_time();

            size_t binary_size = size;
            size_t written = 0;

            while (size > 0) {

                printf("Writing at 0x%X...", (address + written));

                size_t to_read = MIN(size, sizeof(payload));
                memcpy(payload, bin_addr, to_read);

                err = esp_loader_flash_write(payload, to_read);
                if (err != ESP_LOADER_SUCCESS) {
                    ESP_LOGE(TAG, "Packet could not be written! Error %d.", err);
                    return err;
                }

                size -= to_read;
                bin_addr += to_read;
                written += to_read;

                int progress = (int)(((float)written / binary_size) * 100);
                printf("\t(%d %%)\n", progress);
                fflush(stdout);
            };

            ESP_LOGI(TAG, "Wrote %d bytes at 0x%X in %lld ms", binary_size, address, ((esp_timer_get_time() - _startProg) / 1000));

#if CONFIG_SERIAL_FLASHER_MD5_ENABLED
            ESP_LOGI(TAG, "Verifying flash...");
            err = esp_loader_flash_verify();
            if (err == ESP_LOADER_ERROR_UNSUPPORTED_FUNC) {
                ESP_LOGE(TAG, "Does not support flash verify command.");
                return err;
            } else if (err != ESP_LOADER_SUCCESS) {
                ESP_LOGE(TAG, "MD5 does not match. err: %d", err);
                return err;
            }

            ESP_LOGI(TAG, "Hash of data verified");
#endif

            return ESP_LOADER_SUCCESS;

        }


    }
}