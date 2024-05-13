#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    int ledc_channel;
    int ledc_timer;
    int pin_d0;
    int pin_d1;
    // Add other configuration parameters here
} camera_config_t;

extern camera_config_t camera_config;

#endif
