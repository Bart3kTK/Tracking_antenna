

#include "kalman_2d_filter.h"

float filter_kalman_2d(kalman_state_t *kalman_struct, float new_data) {
  if (!kalman_struct->is_initialized) {
    kalman_struct->x = new_data;
    kalman_struct->is_initialized = true;
  }

  kalman_struct->p = kalman_struct->p + kalman_struct->q;

  kalman_struct->k = kalman_struct->p / (kalman_struct->p + kalman_struct->r);
  kalman_struct->x =
      kalman_struct->x + kalman_struct->k * (new_data - kalman_struct->x);
  kalman_struct->p = (1.0f - kalman_struct->k) * kalman_struct->p;

  return kalman_struct->x;
}