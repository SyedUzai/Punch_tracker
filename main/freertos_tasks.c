#include "LSM6DS3.H"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "esp_log.h"
#include "LSM6DS3.h"
#include "freertos_tasks.h"
#include "portmacro.h"
#include "udp_sender.H"




#define TAG "FREERTOS_TASKS"
QueueHandle_t imu_data_queue;
imu_data_t data;



void imu_task(void *arg){
	while(1){
		
		read_gyro_xl_xyz(I2C_ADDR, &data.gyro_x, &data.gyro_y, &data.gyro_z, &data.accel_x, &data.accel_y, &data.accel_z);
		
		xQueueSend(imu_data_queue, &data, portMAX_DELAY);
		vTaskDelay(pdMS_TO_TICKS(20));
	}	
}

void udp_task(void *arg){
	char payload[128];
	
	while(1){
		if(xQueueReceive(imu_data_queue, &data, portMAX_DELAY) == pdPASS){
			snprintf(payload, sizeof(payload), 
			"AX=%.3f AY=%.3f AZ=%.3f GX=%.3f GY=%.3f GZ=%.3f", 
			data.accel_x, data.accel_y, data.accel_z,
            data.gyro_x, data.gyro_y, data.gyro_z);
            
            udp_client_task(payload);
            vTaskDelay(pdMS_TO_TICKS(20));
           
		}	
	}
}

void create_tasks_queue(){
	imu_data_queue = xQueueCreate(IMU_QUEUE_LENGTH, sizeof(imu_data_t));
	if(imu_data_queue == NULL){
		ESP_LOGE("QUEUE", "Failed to create IMU data queue");
	}
	
	xTaskCreate(imu_task, "imu_task", 4096, NULL, 5, NULL);
	xTaskCreate(udp_task, "udp_sender_task", 4096, NULL, 5, NULL);
	
	
	
}