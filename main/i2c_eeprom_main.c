#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <math.h>
#include <string.h>
#include "lwip/sockets.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"


#include "LSM6DS3.h"
#include "freertos_tasks.h"

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "cogeco",
            .password = "Carshampoo123#",
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
    
	//Initializing I2C connection 
	i2c_master_init();
	vTaskDelay(pdMS_TO_TICKS(100));
	
	//Initializing XL, +- 16g max, 416hz high perf, 
	ESP_ERROR_CHECK(lsm6ds3_write_register(I2C_ADDR, CTRL1_XL, 0x64));
	//Initializing block data update until lsb, msb are read, enabled auto increment 
    ESP_ERROR_CHECK(lsm6ds3_write_register(I2C_ADDR, CTRL3_C, 0x44));
    //Initializing Gyro, 250 dps, 416hz high perf
    ESP_ERROR_CHECK(lsm6ds3_write_register(I2C_ADDR, CTRL2_G, 0x6C));
    
    calib_gyro(I2C_ADDR);
    
    create_tasks_queue();
    
}



