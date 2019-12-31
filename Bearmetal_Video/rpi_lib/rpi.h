#ifndef RPILIB_H
#define RPILIB_H

// bss
#include "bss/rpi_bss.h"

// peripherals
#include "peripherals/rpi_peripherals.h"

// gpio
#include "gpio/rpi_gpio.h"

// timer
#include "timer/rpi_timer.h"

// delay
#include "delay/rpi_delay.h"

// serial
#include "serial/rpi_serial.h"

//  video
#include "video/rpi_video.h"

void rpi_init();
#endif
