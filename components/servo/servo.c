#include "servo.h"
#include "freertos/FreeRTOS.h"

static const char *TAG = "SERVO CONTROL";

Servo_t servos[SERVO_COUNT] = {
    [SERVO_PAN] = SERVO_INIT(14), [SERVO_TILT] = SERVO_INIT(21)

};

esp_err_t init_multiple_servos() {
  esp_err_t ret = ESP_OK;
  for (int i = 0; i < SERVO_COUNT; i++) {
    if (servo_init((ServoId_t)i) != EXIT_SUCCESS) {
      ESP_LOGE(TAG, "Failed to initialize servo %d", i);
      ret = ESP_LOG_ERROR;
    }
  }
  return ret;
}

static inline uint32_t angle_to_duty_us(uint8_t angle) {
  return (angle - SERVO_MIN_ANGLE) *
             (SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) /
             (SERVO_MAX_ANGLE - SERVO_MIN_ANGLE) +
         SERVO_MIN_PULSEWIDTH_US;
}

uint16_t servo_init(ServoId_t servo_id) {
  if (servo_id >= SERVO_COUNT) {
    ESP_LOGE(TAG, "Invalid servo ID: %d", servo_id);
    return EXIT_FAILURE;
  }

  Servo_t *servo_ptr = &servos[servo_id];

  // Create timer
  mcpwm_timer_config_t timer_config = {
      .group_id = servo_id, // Use unique group ID for each servo
      .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
      .resolution_hz = SERVO_FREQUENCY_HZ,
      .period_ticks = SERVO_TIMEBASE_PERIOD,
      .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
  };
  ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &servo_ptr->timer));

  // Create operator
  mcpwm_operator_config_t operator_config = {
      .group_id = servo_id, // Operator in the same group as the timer
  };
  ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &servo_ptr->oper));

  // Connect operator to timer
  ESP_ERROR_CHECK(
      mcpwm_operator_connect_timer(servo_ptr->oper, servo_ptr->timer));

  // Create comparator
  mcpwm_comparator_config_t comparator_config = {
      .flags.update_cmp_on_tez = true,
  };
  ESP_ERROR_CHECK(mcpwm_new_comparator(servo_ptr->oper, &comparator_config,
                                       &servo_ptr->comparator));

  // Create generator
  mcpwm_generator_config_t generator_config = {
      .gen_gpio_num = servo_ptr->pwm_pin,
  };
  ESP_ERROR_CHECK(mcpwm_new_generator(servo_ptr->oper, &generator_config,
                                      &servo_ptr->generator));

  // Set the initial compare value, so that the servo will spin to the center
  ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(
      servo_ptr->comparator,
      angle_to_duty_us((SERVO_MAX_ANGLE + SERVO_MIN_ANGLE) / 2)));

  // Set generator action on timer and compare event
  // Go high on counter empty
  ESP_ERROR_CHECK(mcpwm_generator_set_actions_on_timer_event(
      servo_ptr->generator,
      MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP,
                                   MCPWM_TIMER_EVENT_EMPTY,
                                   MCPWM_GEN_ACTION_HIGH),
      MCPWM_GEN_TIMER_EVENT_ACTION_END()));

  // Go low on compare threshold
  ESP_ERROR_CHECK(mcpwm_generator_set_actions_on_compare_event(
      servo_ptr->generator,
      MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP,
                                     servo_ptr->comparator,
                                     MCPWM_GEN_ACTION_LOW),
      MCPWM_GEN_COMPARE_EVENT_ACTION_END()));

  // servo_ptr->close_timer = xTimerCreate(
  //     "CLOSE_SERVO_TIMER", // Timer name
  //     pdMS_TO_TICKS(1000), // Default period (will be updated in move_servo)
  //     pdFALSE,             // One-shot timer (not auto-reload)
  //     (void *)(intptr_t)servo_id, // Timer ID (servo_id)
  //     close_servo_callback        // Callback function
  // );
  // if (servo_ptr->close_timer == NULL) {
  //   ESP_LOGE(TAG, "Failed to create timer for servo %d", servo_id);
  //   return EXIT_FAILURE;
  // }

  mcpwm_timer_enable(servo_ptr->timer);
  mcpwm_timer_start_stop(servo_ptr->timer, MCPWM_TIMER_START_NO_STOP);

  ESP_LOGI(TAG, "Servo %d initialized on GPIO %d", servo_id,
           servo_ptr->pwm_pin);
  return EXIT_SUCCESS;
}

esp_err_t servo_move(ServoId_t servo_id, uint8_t angle) {
  if (servo_id >= SERVO_COUNT) {
    ESP_LOGE(TAG, "Invalid servo ID: %d", servo_id);
    return ESP_LOG_ERROR;
  }

  Servo_t *servo_ptr = &servos[servo_id];
  ESP_LOGI(TAG, "Moving servo[%d] to angle: %d", servo_id, angle);

  // mcpwm_timer_enable(servo_ptr->timer);
  // mcpwm_timer_start_stop(servo_ptr->timer, MCPWM_TIMER_START_NO_STOP);

  if (mcpwm_comparator_set_compare_value(servo_ptr->comparator,
                                         angle_to_duty_us(angle)) != ESP_OK) {
    ESP_LOGE(TAG, "Moving servo %d FAILURE", servo_id);
    return ESP_LOG_ERROR;
  }

  //   vTaskDelay(
  //       pdMS_TO_TICKS(300)); // Short delay to ensure the servo has time to
  //       move
  //   if (angle < SERVO_MAX_ANGLE) {
  //     mcpwm_comparator_set_compare_value(servo_ptr->comparator,
  //                                        angle_to_duty_us(angle + 1));
  //   }
  servo_ptr->angle = angle;

  return ESP_OK;
}