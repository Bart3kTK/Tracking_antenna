#ifndef SERVO_H
#define SERVO_H

#include "driver/mcpwm_prelude.h"
#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

#define SERVO_MIN_PULSEWIDTH_US 900
#define SERVO_MAX_PULSEWIDTH_US 2100
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 120
#define SERVO_FREQUENCY_HZ 1000000
#define SERVO_TIMEBASE_PERIOD 20000 // 20000 ticks, 20ms

#define DEFAULT_POS_ANGLE 90 // Angle of the default position for servos
#define MOVE_WITHOUT_TIMER (uint16_t)0
#define MIN_SERVO_TIMER_TIME_MS 400

typedef enum { SERVO_PAN, SERVO_TILT, SERVO_COUNT } ServoId_t;

typedef struct {
  uint32_t pwm_pin; /**< GPIO connects to the PWM signal line */
  mcpwm_timer_handle_t timer;
  mcpwm_oper_handle_t oper;
  mcpwm_cmpr_handle_t comparator;
  mcpwm_gen_handle_t generator;
  uint8_t angle;
  TimerHandle_t close_timer;
} Servo_t;

#define SERVO_INIT(X)                                                          \
  {                                                                            \
    .pwm_pin = X, .timer = NULL, .oper = NULL, .comparator = NULL,             \
    .generator = NULL                                                          \
  }

extern Servo_t servos[SERVO_COUNT];

uint16_t servo_init(ServoId_t servo_id);
esp_err_t servo_move(ServoId_t servo_id, uint8_t angle);
esp_err_t init_multiple_servos();

#endif