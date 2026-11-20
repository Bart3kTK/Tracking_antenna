#include "system_math.h"
#include "math.h"
#include "ms5611_config.h"
float height_calc(int32_t press_pa, float temp) {
  float temperature = temp + 273.15f;
  float p0 = 101325.0f;
  float h = -29.271769f * temperature * logf((float)press_pa / p0);
  return h;
}