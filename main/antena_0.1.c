#include "gps_wrapper.h"
#include "measure_task.h"
#include "ms5611_config.h"
#include "neo6m.h"
#include "now.h"
#include "now_task.h"
#include "pinout.h"
#include "sensors_data.h"
#include "servo.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <hmc5883l.h>
#include <inttypes.h>
#include <math.h>
#include <mpu6050.h>
#include <ms5611.h>
#include <stdio.h>
#include <string.h>

// void task(void *arg)
// {
// 	gps_start(); //avoid using gps_start() again until and unless driver is
// not de-inited properly. 	 while(1)
// 	 {
// 	  raw_nmea();

//      }
// }

// // use uart_driver_delete(UART_NUM_0) when done with UART to avoid collision
// with other periphrals.

// void app_main(void)
// {

//     xTaskCreatePinnedToCore(&task, "test", 4092*2, NULL, 5, NULL, 1);

// }

// void hmc5883l_test(void *pvParameters)
// {
//     hmc5883l_dev_t dev;
//     memset(&dev, 0, sizeof(hmc5883l_dev_t));

//     ESP_ERROR_CHECK(hmc5883l_init_desc(&dev, 0, 18, 19));
//     ESP_ERROR_CHECK(hmc5883l_init(&dev));

//     ESP_ERROR_CHECK(hmc5883l_set_opmode(&dev, HMC5883L_MODE_CONTINUOUS));
//     ESP_ERROR_CHECK(hmc5883l_set_samples_averaged(&dev, HMC5883L_SAMPLES_8));
//     ESP_ERROR_CHECK(hmc5883l_set_data_rate(&dev, HMC5883L_DATA_RATE_07_50));
//     ESP_ERROR_CHECK(hmc5883l_set_gain(&dev, HMC5883L_GAIN_1090));

//     vTaskDelay(pdMS_TO_TICKS(250));

//     // float minx = 99999;
//     // float miny = 99999;
//     // float minz = 99999;
//     // float maxx = -99999;
//     // float maxy = -99999;
//     // float maxz = -99999;
//     float offsetX = 19.78;
//     float offsetY = -229.54;

//     while (1)
//     {

//         hmc5883l_data_t data;
//         if (hmc5883l_get_data(&dev, &data) == ESP_OK)
//         {

//             /* float is used in printf(). you need non-default configuration
//             in
//              * sdkconfig for ESP8266, which is enabled by default for this
//              * example. see sdkconfig.defaults.esp8266
//              */
//             printf("Magnetic data: X:%.2f mG, Y:%.2f mG, Z:%.2f mG\n",
//             data.x, data.y, data.z);
//             // if(data.x < minx)
//             // {
//             //     minx = data.x;
//             // }
//             // if(data.y < miny)
//             // {
//             //     miny = data.y;
//             // }
//             // if(data.z < minz)
//             // {
//             //     minz = data.z;
//             // }
//             // if(data.x > maxx)
//             // {
//             //     maxx = data.x;
//             // }
//             // if(data.y > maxy)
//             // {
//             //     maxy= data.y;
//             // }
//             // if(data.z > maxz)
//             // {
//             //     maxz = data.z;
//             // }

//             // float offsetX = (maxx + minx) / 2.0f;
//             // float offsetY = (maxy + miny) / 2.0f;
//             // float offsetZ = (maxz + minz) / 2.0f;

//             // printf("Mins: x %.2f y %.2f z %.2f\n", minx, miny, minz);
//             // printf("Maxs: x %.2f y %.2f z %.2f\n", maxx, maxy, maxz);
//             // printf("Offsets : x %.2f y %.2f z %.2f\n", offsetX, offsetY,
//             offsetZ);

//             float xCal = data.x - offsetX;
//             float yCal = data.y - offsetY;
//             float angle = atan2f(xCal, yCal)*180/M_PI;
//             if(angle < 0) angle += 360;
//             printf("Angle %.2f\n", angle);

//         }
//         else
//             printf("Could not read HMC5883L data\n");

//         vTaskDelay(pdMS_TO_TICKS(250));
//     }
// }

// void app_main()
// {
//     gpio_pullup_en(18);
//     gpio_pullup_en(19);
//     ESP_ERROR_CHECK(i2cdev_init());
//     xTaskCreatePinnedToCore(hmc5883l_test, "hmc5883l_test", 4096, NULL, 5,
//     NULL, 0);
// }

// static const char *TAG = "mpu6050_test";

// void mpu6050_test(void *pvParameters)
// {
//     mpu6050_dev_t dev = { 0 };

//     ESP_ERROR_CHECK(mpu6050_init_desc(&dev, MPU6050_I2C_ADDRESS_LOW, 0, 18,
//     19));

//     while (1)
//     {
//         esp_err_t res = i2c_dev_probe(&dev.i2c_dev, I2C_DEV_WRITE);
//         if (res == ESP_OK)
//         {
//             ESP_LOGI(TAG, "Found MPU60x0 device");
//             break;
//         }
//         ESP_LOGE(TAG, "MPU60x0 not found");
//         vTaskDelay(pdMS_TO_TICKS(1000));
//     }

//     ESP_ERROR_CHECK(mpu6050_init(&dev));

//     ESP_LOGI(TAG, "Accel range: %d", dev.ranges.accel);
//     ESP_LOGI(TAG, "Gyro range:  %d", dev.ranges.gyro);

//     while (1)
//     {
//         float temp;
//         mpu6050_acceleration_t accel = { 0 };
//         mpu6050_rotation_t rotation = { 0 };

//         ESP_ERROR_CHECK(mpu6050_get_temperature(&dev, &temp));
//         ESP_ERROR_CHECK(mpu6050_get_motion(&dev, &accel, &rotation));

//         ESP_LOGI(TAG,
//         "**********************************************************************");
//         ESP_LOGI(TAG, "Acceleration: x=%.4f   y=%.4f   z=%.4f", accel.x,
//         accel.y, accel.z); ESP_LOGI(TAG, "Rotation:     x=%.4f   y=%.4f
//         z=%.4f", rotation.x, rotation.y, rotation.z); ESP_LOGI(TAG,
//         "Temperature:  %.1f", temp);

//         vTaskDelay(pdMS_TO_TICKS(100));
//     }
// }

void app_main() {
  init_wifi_and_espnow();
  ESP_ERROR_CHECK(i2cdev_init());
  init_sensors_data();
  init_multiple_servos();
  uint8_t angle = 0;
  ms5611_config_init();
  start_barometer_task();
  // start_now_send_task();
  // gps_init();

  // while (true)
  // {
  //     print_raw_data();
  //     vTaskDelay(pdMS_TO_TICKS(1000));
  // }

  // // task
  // ESP_ERROR_CHECK(i2cdev_init());

  // xTaskCreate(mpu6050_test, "mpu6050_test", configMINIMAL_STACK_SIZE * 6,
  // NULL, 5, NULL);
}

// i2c - sda 8 scl 9
//  TX - gpio17 RX gpio 18
//  pwm 14 i 21