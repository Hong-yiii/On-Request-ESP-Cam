#ifndef CONFIG_H
#define CONFIG_H

#include "esp_camera.h"

// typedef struct {
//     int pin_xclk;
//     int pin_sscb_sda;
//     int pin_sscb_scl;

//     int pin_d0;
//     int pin_d1;
//     int pin_d2;
//     int pin_d3;
//     int pin_d4;
//     int pin_d5;
//     int pin_d6;
//     int pin_d7;
//     int pin_vsync;
//     int pin_href;
//     int pin_pclk;

//     int xclk_freq_hz;
//     int pixel_format;
//     int frame_size;
//     int jpeg_quality;
//     int fb_count;
//     // Add other configuration parameters here
// } camera_config_t;

extern camera_config_t camera_config;

#endif
