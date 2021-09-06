#ifndef __main_h
#define __main_h

#include <intrinsics.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "chip.h"
#include "board.h"

#include "servo_control.h"
#include "timers.h"
#include "one_wire.h"
#include "shutter.h"
#include "spi.h"
#include "memory.h"
#include "i2c.h"

#define TICKRATE_HZ     1000
#define POSITION_TIMEOUT        1000

#endif