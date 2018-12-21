/*
 * display.h
 * This file contains all globals, constants, and function prototypes for the display
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <msp430.h>
#include <stdint.h>
#include "grlib.h"

#include "LcdDriver/Sharp96x96.h"
#include "LcdDriver/HAL_MSP_EXP430FR5529_Sharp96x96.h"

// Globals
extern tContext g_sContext; // user defined type used by graphics library


// Function Prototypes
void configure_display(void);

#endif
