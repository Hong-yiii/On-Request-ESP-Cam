#include "configs.h"

// Initialize the camera configuration
camera_config_t camera_config = {
    .ledc_channel = // LEDC_CHANNEL_0,
    .ledc_timer = // LEDC_TIMER_0,
    .pin_d0 = // Y2_GPIO_NUM,
    .pin_d1 = // Y3_GPIO_NUM,
    // Initialize other parameters as necessary
};
