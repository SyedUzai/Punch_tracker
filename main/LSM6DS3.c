#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "LSM6DS3.h"
#include <math.h>


gyro_bias_t gyro_bias ={0};


uint8_t get_i2c_address(void) {
    gpio_set_direction(SA0_GPIO, GPIO_MODE_INPUT);
    int level = gpio_get_level(SA0_GPIO); 
    return level ? 0x6B : 0x6A;
}

void i2c_master_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));
}

uint8_t lsm6ds3_read_register(uint8_t i2c_addr, uint8_t reg_addr) {
    uint8_t data = 0;

    esp_err_t ret = i2c_master_write_read_device(
        I2C_MASTER_NUM, i2c_addr,
        &reg_addr, 1,
        &data, 1,
        pdMS_TO_TICKS(1000)
    );
    ESP_ERROR_CHECK(ret);
    return data;
}

void read_gyro_xyz(uint8_t i2c_addr,float_t *raw_gyro_x, float_t *raw_gyro_y, float_t *raw_gyro_z) {
    uint8_t buf[6];

    esp_err_t ret = i2c_master_write_read_device(
        I2C_MASTER_NUM, i2c_addr,
        &((uint8_t){OUTX_L_G}), 1,
        buf, 6,
        pdMS_TO_TICKS(1000)
    );
    ESP_ERROR_CHECK(ret);
    
    int16_t raw_x = (int16_t)((buf[1] << 8) | buf[0]);
    int16_t raw_y = (int16_t)((buf[3] << 8) | buf[2]);
    int16_t raw_z = (int16_t)((buf[5] << 8) | buf[4]);
    
    const float gyro_sens = 0.07f;  // ±2000 dps sensitivity


	*raw_gyro_x =  raw_x * gyro_sens;
	*raw_gyro_y =  raw_y * gyro_sens;
	*raw_gyro_z =  raw_z * gyro_sens;

    ESP_LOGI("GYRO",
        "X=%f dps  Y=%f dps  Z=%f dps",
        *raw_gyro_x,
        *raw_gyro_y,
        *raw_gyro_z
    );
}

void read_gyro_xl_xyz(uint8_t i2c_addr, float_t *gyro_x, float_t *gyro_y, float_t *gyro_z, float_t *xl_x, float_t *xl_y, float_t *xl_z) {
    uint8_t buf[12];

    esp_err_t ret = i2c_master_write_read_device(
        I2C_MASTER_NUM, i2c_addr,
        &((uint8_t){OUTX_L_G}), 1,
        buf, 12,
        pdMS_TO_TICKS(10)
    );
    ESP_ERROR_CHECK(ret);
    
    int16_t raw_x_gyro = (int16_t)((buf[1] << 8) | buf[0]);
    int16_t raw_y_gyro = (int16_t)((buf[3] << 8) | buf[2]);
    int16_t raw_z_gyro = (int16_t)((buf[5] << 8) | buf[4]);
    
    
    int16_t raw_x_xl = (int16_t)((buf[7] << 8) | buf[6]);
    int16_t raw_y_xl = (int16_t)((buf[9] << 8) | buf[8]);
    int16_t raw_z_xl = (int16_t)((buf[11] << 8) | buf[10]);
    
    const float gyro_sens = 0.07f;  // ±2000 dps sensitivity
	const float xl_sens = 0.000488f;  // ±16 g sensitivity

  	*gyro_x = (raw_x_gyro * gyro_sens) - gyro_bias.x;
    *gyro_y = (raw_y_gyro * gyro_sens) - gyro_bias.y;
    *gyro_z = (raw_z_gyro * gyro_sens) - gyro_bias.z;
    *xl_x = raw_x_xl * xl_sens;
    *xl_y = raw_y_xl * xl_sens;
    *xl_z = raw_z_xl * xl_sens;
    
    
     ESP_LOGI("GYRO:",
        "X=%f dps  Y=%f dps  Z=%f dps",
        	*gyro_x,
        	*gyro_y,
       		*gyro_z
    );
    
    
     ESP_LOGI("ACCEL:",
        "X=%f G  Y=%f G  Z=%f G",
       *xl_x,
       *xl_y,
       *xl_z
    );
    
    
}


esp_err_t lsm6ds3_write_register(uint8_t addr, uint8_t reg, uint8_t value) {
    uint8_t data[2] = { reg, value };
    return i2c_master_write_to_device(
        I2C_MASTER_NUM, addr, data, 2, pdMS_TO_TICKS(1000)
    );
}

void calib_gyro(uint8_t i2c_addr){
	float sum_x = 0.0f;
	float sum_y = 0.0f;
	float sum_z = 0.0f;
	
	for( int i = 0; i < CALIBRATION_SAMPLES; i++){
		float_t gyro_bias_x = 0, gyro_bias_y = 0, gyro_bias_z = 0;
		read_gyro_xyz(I2C_ADDR, &gyro_bias_x, &gyro_bias_y, &gyro_bias_z);
		
		sum_x += gyro_bias_x;
		sum_y += gyro_bias_y;
		sum_z += gyro_bias_z;
		
		vTaskDelay(pdMS_TO_TICKS(2));  // ~500 Hz sample rate
		ESP_LOGI("Hi", "Counter %d", i);
	}
	
	gyro_bias.x =  sum_x / CALIBRATION_SAMPLES;
	gyro_bias.y =  sum_y / CALIBRATION_SAMPLES;
	gyro_bias.z =  sum_z / CALIBRATION_SAMPLES;
	
	ESP_LOGI("CALIBRATION", "Gyro bias: X=%.2f, Y=%.2f, Z=%.2f", gyro_bias.x, gyro_bias.y, gyro_bias.z);
}