#ifndef KALMAN_2D_H
#define KALMAN_2D_H

#include <stdbool.h>

typedef struct {
  float q; // Szum procesu (jak bardzo ufasz dynamice zmian, np. 0.01)
  float r; // Szum pomiarowy (jak bardzo szumi czujnik, np. 0.1)
  float x; // Estymowana wartość (przefiltrowana wysokość)
  float p; // Błąd estymacji
  float k; // Wzmocnienie Kalmana
  bool is_initialized;
} kalman_state_t;

float filter_kalman_2d(kalman_state_t *kalman_struct, float new_data);

#endif