#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "esp_err.h"

sdmmc_card_t *sd_card;

esp_err_t is_mounted;

void sd_init(void)
{
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = SPI2_HOST;

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = 27;
    slot_config.host_id = SPI2_HOST;
    host.max_freq_khz = 400;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
    };

    is_mounted = esp_vfs_fat_sdspi_mount("/main", &host, &slot_config, &mount_config, &sd_card);

    if (is_mounted != ESP_OK) {
        printf("SD mount failed: %d\n", is_mounted);
    } else {
        printf("SD mounted OK\n");
    }

}
