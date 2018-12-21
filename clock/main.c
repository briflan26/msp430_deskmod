/************** Lab 3 ******************/
/************** Jake Barefoot, YaYa Brown   ******************/
/***************************************************/

#include <msp430.h>
#include <math.h>
#include <string.h>
#include "display.h"

/* Peripherals.c and .h are where the functions that implement
 * the LEDs and keypad, etc are. It is often useful to organize
 * your code by putting like functions together in files.
 * You include the header associated with that file(s)
 * into the main file of your project. */
#include "peripherals.h"

// Function Prototypes
void swDelay(char numLoops);
void display_date(void);
void display_time(void);
void display_temp(int temp);
void run_TimerA2(void);
void launch_button(void);

// Declare globals here

//globals used for keeping track of interrupt values
unsigned long utc_cnt = 0;
unsigned int adc_inTemp = 0;
unsigned int adc_scroll = 0;

//variables used for calculating temperature (based on average of previous 10 codes)
int temp[10] = { 0 };
int index2 = 0;
int temp_total;
unsigned long display = 0;
int avg;

//variables used for the display date (includes the month and day)
unsigned int day_timer = 0;
char *month = "MAR";

//variables used to display time (hours minutes and seconds)
unsigned int hours = 0;
unsigned int minutes = 0;
unsigned int seconds = 0;

// Main
void main(void)

{
    //configure adc registers, button registers, and start the timer
    WDTCTL = WDTPW | WDTHOLD;
    configure_display();
    run_TimerA2();
    //launch_button();

    while (true)
    {
        // draw boxes
        Graphics_drawLineV(&g_sContext, 4, 4, 90);
        Graphics_drawLineV(&g_sContext, 90, 4, 90);
        Graphics_drawLineH(&g_sContext, 4, 90, 4);
        Graphics_drawLineH(&g_sContext, 4, 90, 90);
        Graphics_drawLineH(&g_sContext, 4, 90, 45);

        // draw rects
        Graphics_fillRectangle(const Graphics_Context *context, const Graphics_Rectangle *rect);
        Graphics_flushBuffer(&g_sContext);
    }

}

void display_date(void)
{

//March 14 starting day

//determine the day and month based on the value of utc (take into account the number of days
//since from the start of utc to the current month)
    if (utc_cnt < (17 * 86400))
    {
        day_timer = (utc_cnt / 86400) + 14;
    }
    else if (utc_cnt < (47 * 86400))
    {
        month = "APR";
        day_timer = ((utc_cnt - (17 * 86400)) / 86400) + 1;
    }
    else if (utc_cnt < (78 * 86400))
    {
        month = "MAY";
        day_timer = ((utc_cnt - (47 * 86400)) / 86400) + 1;
    }
    else if (utc_cnt < (108 * 86400))
    {
        month = "JUN";
        day_timer = ((utc_cnt - (78 * 86400)) / 86400) + 1;
    }
    else if (utc_cnt < (139 * 86400))
    {
        month = "JUL";
        day_timer = ((utc_cnt - (108 * 86400)) / 86400) + 1;
    }
    else if (utc_cnt < (170 * 86400))
    {
        month = "AUG";
        day_timer = ((utc_cnt - (139 * 86400)) / 86400) + 1;
    }
    else if (utc_cnt < (200 * 86400))
    {
        month = "SEP";
        day_timer = ((utc_cnt - (170 * 86400)) / 86400) + 1;
    }
    else if (utc_cnt < (231 * 86400))
    {
        month = "OCT";
        day_timer = ((utc_cnt - (200 * 86400)) / 86400) + 1;
    }
    else if (utc_cnt < (261 * 86400))
    {
        month = "NOV";
        day_timer = ((utc_cnt - (231 * 86400)) / 86400) + 1;
    }
    else if (utc_cnt < (292 * 86400))
    {
        month = "DEC";
        day_timer = ((utc_cnt - (261 * 86400)) / 86400) + 1;
    }
    else if (utc_cnt < (323 * 86400))
    {
        month = "JAN";
        day_timer = ((utc_cnt - (292 * 86400)) / 86400) + 1;
    }
    else if (utc_cnt < (351 * 86400)) //assuming not a leap year
    {
        month = "FEB";
        day_timer = ((utc_cnt - (323 * 86400)) / 86400) + 1;
    }
    else
    {
        month = "MAR";
        day_timer = ((utc_cnt - (351 * 86400)) / 86400) + 1;
    }

//display the month and day
    Graphics_drawStringCentered(&g_sContext, month, AUTO_STRING_LENGTH, 28, 30,
    OPAQUE_TEXT);

    char day[3];

    day[0] = (day_timer / 10) + 48;
    day[1] = (day_timer % 10) + 48;
    day[2] = '\0';
    Graphics_drawStringCentered(&g_sContext, day, AUTO_STRING_LENGTH, 50, 30,
    OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

void display_time(void)
{
//get the remaining hours minutes and seconds (remove all full days from utc_cnt)
    unsigned int day_time = utc_cnt % 86400;
    char hms[9];

//calculate hours minutes and seconds (1 utc_cnt = 1 second)
    hours = day_time / 3600;
    hms[0] = day_time / 36000 + 48;
    day_time %= 36000;
    hms[1] = day_time / 3600 + 48;
    day_time %= 3600;
    hms[2] = ':';
    minutes = day_time / 60;
    hms[3] = day_time / 600 + 48;
    day_time %= 600;
    hms[4] = day_time / 60 + 48;
    day_time %= 60;
    hms[5] = ':';
    seconds = day_time;
    hms[6] = day_time / 10 + 48;
    day_time %= 10;
    hms[7] = day_time + 48;
    hms[8] = '\0';

    Graphics_drawStringCentered(&g_sContext, hms, AUTO_STRING_LENGTH, 50, 40,
    OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

void run_TimerA2(void)
{
// Use ACLK, 16 Bit, up mode, 1 divider
    TA2CTL = TASSEL_1 + MC_1 + ID_0;
    TA2CCR0 = 32767; // 32767+1 = 32768 ACLK tics = 1 seconds
    TA2CCTL0 = CCIE; // TA2CCR0 interrupt enabled
}

void stoptimerA2(int reset)
{
//stops timer (reset count to 0 optional)
    TA2CTL = MC_0;
    TA2CCTL0 &= ~CCIE;
    if (reset)
        utc_cnt = 0;
}

// Timer A2 interrupt service routine
#pragma vector=TIMER2_A0_VECTOR
__interrupt void TimerA2_ISR(void)
{
    utc_cnt++;

//display date and time
    display_date();
    display_time();
}

void launch_button(void)
{
//configure launch buttons (right ends edit mode, left begns edit mode and changes edit state)
    P2SEL &= ~(BIT1);
    P1SEL &= ~(BIT1);

    P2DIR &= ~(BIT1);
    P1DIR &= ~(BIT1);

    P2REN |= BIT1;
    P1REN |= BIT1;

    P2OUT |= BIT1;
    P1OUT |= BIT1;
}

void swDelay(char numLoops)
{
// This function is a software delay. It performs
// useless loops to waste a bit of time
//
// Input: numLoops = number of delay loops to execute
// Output: none
//
// smj, ECE2049, 25 Aug 2013

    volatile unsigned int i, j;  // volatile to prevent removal in optimization
                                 // by compiler. Functionally this is useless code

    for (j = 0; j < numLoops; j++)
    {
        i = 50000;                 // SW Delay
        while (i > 0)               // could also have used while (i)
            i--;
    }
}
