#ifndef MAIN_IMU_LSM6DS3_h_
#define MAIN_IMU_LSM6DS3_h_


#include "esp_err.h"
#include <math.h>
#include <stdint.h>


#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_SCL_IO 1
#define I2C_MASTER_SDA_IO 3
#define I2C_MASTER_FREQ_HZ 400000

#define OUTX_L_XL 0x28
#define OUTX_L_G 0x22
#define CTRL1_XL 0x10
#define CTRL2_G 0x11
#define CTRL3_C 0x12


#define I2C_ADDR 0x6B
#define SA0_GPIO 2
#define CALIBRATION_SAMPLES 2000

typedef struct {
    float x;
    float y;
    float z;
} gyro_bias_t;

typedef struct {
    float accel_x, accel_y, accel_z;
    float gyro_x,  gyro_y,  gyro_z;
} imu_data_t;

// Global variable for gyro bias
extern gyro_bias_t gyro_bias;

// Function declarations
uint8_t get_i2c_address(void);
void i2c_master_init(void);

uint8_t lsm6ds3_read_register(uint8_t i2c_addr, uint8_t reg_addr);
esp_err_t lsm6ds3_write_register(uint8_t addr, uint8_t reg, uint8_t value);

void read_gyro_xyz(uint8_t i2c_addr, float_t *raw_gyro_x, float_t *raw_gyro_y, float_t *raw_gyro_z);
void read_gyro_xl_xyz(uint8_t i2c_addr, float_t *gyro_x, float_t *gyro_y, float_t *gyro_z,
                      float_t *xl_x, float_t *xl_y, float_t *xl_z);

void calib_gyro(uint8_t i2c_addr);


#endif /* MAIN_IMU_LSM6DS3_h_ */
