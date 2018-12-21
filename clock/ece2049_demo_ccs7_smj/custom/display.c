/*
 * display.c
 * This file will contain all display related functions.
 */

#include "custom/display.h"

void configure_display(void)
{
    // Enable use of external clock crystals
     P5SEL |= (BIT5|BIT4|BIT3|BIT2);

    // Initialize the display peripheral
    Sharp96x96_Init();

    // Configure the graphics library to use this display.
    // The global g_sContext is a data structure containing information the library uses
    // to send commands for our particular display.
    // You must pass this parameter to each graphics library function so it knows how to
    // communicate with our display.
    Graphics_initContext(&g_sContext, &g_sharp96x96LCD);


    Graphics_setForegroundColor(&g_sContext, ClrBlack);
    Graphics_setBackgroundColor(&g_sContext, ClrWhite);
    Graphics_setFont(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
    Graphics_flushBuffer(&g_sContext);
}
