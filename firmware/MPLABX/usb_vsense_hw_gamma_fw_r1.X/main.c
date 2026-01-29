/*
    [USB VSense] - [USB Voltage Indicator main firmware]
    Copyright (C) [2026] [Meticulous Technologies]

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/* ========================================================================== */
/* main.c USB Voltage indicator (USB VSense)                                  */
/* ========================================================================== */
/* Description: Main firmware (intended for production)                       */
/*                                                                            */
/* ========================================================================== */
/* Requires:  Modules: GPIO                                                   */
/*            main.h (part of main module)                                    */
/*                                                                            */
/* ========================================================================== */
/* author:              S. BHONWAL                                            */
/* company:             Meticulous Technologies                               */
/* library:                                                                   */
/* project:             005 USB Volt Indicator (USB VSense)                   */
/* hardware version     Beta, Gamma                                           */
/* version:             1                                                     */
/* revision:            0                                                     */
/* date:                13-09-2024                                            */
/* changes:                                                                   */
/* ========================================================================== */

#include "main.h"
#include "gpio.h"
#include <stdint.h>
#include <math.h>
#include <xc.h>
#include <avr/cpufunc.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define ADC_VAL_SCHMTTRIG   0x01 // 21.4mV LOW and 50.3mV HIGH  Schmitt trigger hysteresis voltage

#define ADC_VAL_MAX   0x3FF

#define HIGH_RNG_SW_VOLT    21.65 //Switch to high range at this voltage
#define LOW_RNG_SW_VOLT     21.35 //Switch to low range at this voltage

#define ADC_VREF        4.3

#define LOW_VOLT_DIV    5.1 //ADC LOW VOLTAGE DIVIDER RATIO (VOUT/VIN)
#define HIGH_VOLT_DIV   12  //ADC HIGH VOLTAGE DIVIDER RATIO (VOUT/VIN)

#define ADC_CONV_FACT_LOW_RANGE   ((float)ADC_VAL_MAX/((float)ADC_VREF*(float)LOW_VOLT_DIV))

#define ADC_CONV_FACT_HIGH_RANGE   ((float)ADC_VAL_MAX/((float)ADC_VREF*(float)HIGH_VOLT_DIV))

#define ADD_05P(a)                  ((float)a+((float)a*(float)0.05))
#define SUB_05P(a)                  ((float)a-((float)a*(float)0.05))
#define ADD_10P(a)                  ((float)a+((float)a*(float)0.10))
#define SUB_10P(a)                  ((float)a-((float)a*(float)0.10))

#define BLINK_TIMER_START()         TCB0.CTRLA |= TCB_ENABLE_bm
#define BLINK_TIMER_STOP()          TCB0.CTRLA &= ~(TCB_ENABLE_bm)
#define BLINK_TIMER_CNT_RESET()     TCB0.CNT = 0x0000
#define BLINK_TIMER_CNT_FAST        0x0AAA
#define BLINK_TIMER_CNT_SLOW        0x2000

#define BLINK_TIMER_BLINK_RATE(a)   TCB0.CCMP = a   

#define START_SEQ_DELAY_MS          150

#define ADC_START()     ADC0.COMMAND |= ADC_STCONV_bm

void start_sequence(void);

static void blink_timer_init(void);

static void adc_init(void);

static volatile uint16_t adc_res_temp = 0x0000;

static const uint16_t winlt_05v_10p_low = 0x0000;
static const uint16_t winht_05v_10p_low =
        round(ADC_CONV_FACT_LOW_RANGE * SUB_10P(5)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_05v_05p_low =
        round(ADC_CONV_FACT_LOW_RANGE * SUB_10P(5)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_05v_05p_low =
        round(ADC_CONV_FACT_LOW_RANGE * SUB_05P(5)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_05v_norm =
        round(ADC_CONV_FACT_LOW_RANGE * SUB_05P(5)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_05v_norm =
        round(ADC_CONV_FACT_LOW_RANGE * ADD_05P(5)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_05v_05p_high =
        round(ADC_CONV_FACT_LOW_RANGE * ADD_05P(5)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_05v_05p_high =
        round(ADC_CONV_FACT_LOW_RANGE * ADD_10P(5)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_05v_10p_high =
        round(ADC_CONV_FACT_LOW_RANGE * ADD_10P(5)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_05v_10p_high =
        round(ADC_CONV_FACT_LOW_RANGE * (float) 7) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_09v_10p_low =
        round(ADC_CONV_FACT_LOW_RANGE * (float) 7) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_09v_10p_low =
        round(ADC_CONV_FACT_LOW_RANGE * SUB_10P(9)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_09v_05p_low =
        round(ADC_CONV_FACT_LOW_RANGE * SUB_10P(9)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_09v_05p_low =
        round(ADC_CONV_FACT_LOW_RANGE * SUB_05P(9)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_09v_norm =
        round(ADC_CONV_FACT_LOW_RANGE * SUB_05P(9)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_09v_norm =
        round(ADC_CONV_FACT_LOW_RANGE * ADD_05P(9)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_09v_05p_high =
        round(ADC_CONV_FACT_LOW_RANGE * ADD_05P(9)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_09v_05p_high =
        round(ADC_CONV_FACT_LOW_RANGE * ADD_10P(9)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_09v_10p_high =
        round(ADC_CONV_FACT_LOW_RANGE * ADD_10P(9)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_09v_10p_high =
        round(ADC_CONV_FACT_LOW_RANGE * (float) 10.5) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_12v_10p_low =
        round(ADC_CONV_FACT_LOW_RANGE * (float) 10.5) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_12v_10p_low =
        round(ADC_CONV_FACT_LOW_RANGE * SUB_10P(12)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_12v_05p_low =
        round(ADC_CONV_FACT_LOW_RANGE * SUB_10P(12)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_12v_05p_low =
        round(ADC_CONV_FACT_LOW_RANGE * SUB_05P(12)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_12v_norm =
        round(ADC_CONV_FACT_LOW_RANGE * SUB_05P(12)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_12v_norm =
        round(ADC_CONV_FACT_LOW_RANGE * ADD_05P(12)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_12v_05p_high =
        round(ADC_CONV_FACT_LOW_RANGE * ADD_05P(12)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_12v_05p_high =
        round(ADC_CONV_FACT_LOW_RANGE * ADD_10P(12)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_12v_10p_high =
        round(ADC_CONV_FACT_LOW_RANGE * ADD_10P(12)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_12v_10p_high =
        round(ADC_CONV_FACT_LOW_RANGE * (float) 13.5) + ADC_VAL_SCHMTTRIG;

//static const uint16_t winlt_15v_10p_low;
//static const uint16_t winht_15v_10p_low;

static const uint16_t winlt_15v_05p_low =
        round(ADC_CONV_FACT_LOW_RANGE * (float) 13.5) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_15v_05p_low =
        round(ADC_CONV_FACT_LOW_RANGE * SUB_05P(15)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_15v_norm =
        round(ADC_CONV_FACT_LOW_RANGE * SUB_05P(15)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_15v_norm =
        round(ADC_CONV_FACT_LOW_RANGE * ADD_05P(15)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_15v_05p_high =
        round(ADC_CONV_FACT_LOW_RANGE * ADD_05P(15)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_15v_05p_high =
        round(ADC_CONV_FACT_LOW_RANGE * ADD_10P(15)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_15v_10p_high =
        round(ADC_CONV_FACT_LOW_RANGE * ADD_10P(15)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_15v_10p_high =
        round(ADC_CONV_FACT_LOW_RANGE * (float) 17.5) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_20v_10p_low =
        round(ADC_CONV_FACT_LOW_RANGE * (float) 17.5) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_20v_10p_low =
        round(ADC_CONV_FACT_LOW_RANGE * SUB_10P(20)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_20v_05p_low =
        round(ADC_CONV_FACT_LOW_RANGE * SUB_10P(20)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_20v_05p_low =
        round(ADC_CONV_FACT_LOW_RANGE * SUB_05P(20)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_20v_norm =
        round(ADC_CONV_FACT_LOW_RANGE * SUB_05P(20)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_20v_norm =
        round(ADC_CONV_FACT_LOW_RANGE * ADD_05P(20)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_20v_05p_high_low_range =
        round(ADC_CONV_FACT_LOW_RANGE * ADD_05P(20)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_20v_05p_high_low_range =
        round(ADC_CONV_FACT_LOW_RANGE * (float) HIGH_RNG_SW_VOLT) - 1;

static const uint16_t winlt_20v_05p_high_high_range =
        round(ADC_CONV_FACT_HIGH_RANGE * (float) LOW_RNG_SW_VOLT);
static const uint16_t winht_20v_05p_high_high_range =
        round(ADC_CONV_FACT_HIGH_RANGE * ADD_10P(20)) + ADC_VAL_SCHMTTRIG;


static const uint16_t winlt_20v_10p_high =
        round(ADC_CONV_FACT_HIGH_RANGE * ADD_10P(20)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_20v_10p_high =
        round(ADC_CONV_FACT_HIGH_RANGE * (float) 24) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_28v_10p_low =
        round(ADC_CONV_FACT_HIGH_RANGE * (float) 24) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_28v_10p_low =
        round(ADC_CONV_FACT_HIGH_RANGE * SUB_10P(28)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_28v_05p_low =
        round(ADC_CONV_FACT_HIGH_RANGE * SUB_10P(28)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_28v_05p_low =
        round(ADC_CONV_FACT_HIGH_RANGE * SUB_05P(28)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_28v_norm =
        round(ADC_CONV_FACT_HIGH_RANGE * SUB_05P(28)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_28v_norm =
        round(ADC_CONV_FACT_HIGH_RANGE * ADD_05P(28)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_28v_05p_high =
        round(ADC_CONV_FACT_HIGH_RANGE * ADD_05P(28)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_28v_05p_high =
        round(ADC_CONV_FACT_HIGH_RANGE * ADD_10P(28)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_28v_10p_high =
        round(ADC_CONV_FACT_HIGH_RANGE * ADD_10P(28)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_28v_10p_high =
        round(ADC_CONV_FACT_HIGH_RANGE * (float) 32) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_36v_10p_low =
        round(ADC_CONV_FACT_HIGH_RANGE * (float) 32) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_36v_10p_low =
        round(ADC_CONV_FACT_HIGH_RANGE * SUB_10P(36)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_36v_05p_low =
        round(ADC_CONV_FACT_HIGH_RANGE * SUB_10P(36)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_36v_05p_low =
        round(ADC_CONV_FACT_HIGH_RANGE * SUB_05P(36)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_36v_norm =
        round(ADC_CONV_FACT_HIGH_RANGE * SUB_05P(36)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_36v_norm =
        round(ADC_CONV_FACT_HIGH_RANGE * ADD_05P(36)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_36v_05p_high =
        round(ADC_CONV_FACT_HIGH_RANGE * ADD_05P(36)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_36v_05p_high =
        round(ADC_CONV_FACT_HIGH_RANGE * ADD_10P(36)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_36v_10p_high =
        round(ADC_CONV_FACT_HIGH_RANGE * ADD_10P(36)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_36v_10p_high =
        round(ADC_CONV_FACT_HIGH_RANGE * (float) 42) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_48v_10p_low =
        round(ADC_CONV_FACT_HIGH_RANGE * (float) 42) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_48v_10p_low =
        round(ADC_CONV_FACT_HIGH_RANGE * SUB_10P(48)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_48v_05p_low =
        round(ADC_CONV_FACT_HIGH_RANGE * SUB_10P(48)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_48v_05p_low =
        round(ADC_CONV_FACT_HIGH_RANGE * SUB_05P(48)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_48v_norm =
        round(ADC_CONV_FACT_HIGH_RANGE * SUB_05P(48)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_48v_norm =
        round(ADC_CONV_FACT_HIGH_RANGE * ADD_05P(48)) + ADC_VAL_SCHMTTRIG;

static const uint16_t winlt_48v_05p_high =
        round(ADC_CONV_FACT_HIGH_RANGE * ADD_05P(48)) - ADC_VAL_SCHMTTRIG;
static const uint16_t winht_48v_05p_high = ADC_VAL_MAX;

static PORT_t *tgl_port;
static uint8_t tgl_pin_bm;

int main(void)
{
    //Change FCPU (Main clock)
    uint8_t temp = CLKCTRL.MCLKCTRLB;
    temp &= ~(CLKCTRL_PEN_bm); //disable Main Clock prescaler
    ccp_write_io((void*) &CLKCTRL.MCLKCTRLB, temp);
    while (!(CLKCTRL.MCLKSTATUS & CLKCTRL_OSC20MS_bm)); //wait for OSC20 stable

    temp = CLKCTRL.MCLKCTRLA;
    temp |= CLKCTRL_CLKSEL_OSCULP32K_gc; //Select OSC32KS as main OSC
    ccp_write_io((void*) &CLKCTRL.MCLKCTRLA, temp);
    while (!(CLKCTRL.MCLKSTATUS & CLKCTRL_OSC32KS_bm)); //wait for OSC32KS stable
    
    temp = CLKCTRL.OSC32KCTRLA;
    temp |= CLKCTRL_RUNSTDBY_bm;
    ccp_write_io((void*) &CLKCTRL.OSC32KCTRLA, temp);

    cli(); //Disable global interrupts

    gpio_init();
    blink_timer_init();
    tgl_port = 0x0000;
    tgl_pin_bm = 0x00;

    start_sequence();

    adc_init();
    ADC_START();

    CPUINT.LVL1VEC = ADC0_WCOMP_vect_num;
    sei(); //Enable global interrupts

    
    //BLINK_TIMER_BLINK_RATE(BLINK_TIMER_CNT_SLOW);
    //BLINK_TIMER_START();
    //asm("nop");

    while (1)
    {
        //nothing to do here!
    }

    return 0x00;
}

void start_sequence()
{
    LED_48V_PORT.OUTSET = LED_48V_PIN_bm;
    _delay_ms(START_SEQ_DELAY_MS);
    LED_48V_PORT.OUTCLR = LED_48V_PIN_bm;
    LED_36V_PORT.OUTSET = LED_36V_PIN_bm;
    _delay_ms(START_SEQ_DELAY_MS);
    LED_36V_PORT.OUTCLR = LED_36V_PIN_bm;
    LED_28V_PORT.OUTSET = LED_28V_PIN_bm;
    _delay_ms(START_SEQ_DELAY_MS);
    LED_28V_PORT.OUTCLR = LED_28V_PIN_bm;
    LED_20V_PORT.OUTSET = LED_20V_PIN_bm;
    _delay_ms(START_SEQ_DELAY_MS);
    LED_20V_PORT.OUTCLR = LED_20V_PIN_bm;
    LED_15V_PORT.OUTSET = LED_15V_PIN_bm;
    _delay_ms(START_SEQ_DELAY_MS);
    LED_15V_PORT.OUTCLR = LED_15V_PIN_bm;
    LED_12V_PORT.OUTSET = LED_12V_PIN_bm;
    _delay_ms(START_SEQ_DELAY_MS);
    LED_12V_PORT.OUTCLR = LED_12V_PIN_bm;
    LED_09V_PORT.OUTSET = LED_09V_PIN_bm;
    _delay_ms(START_SEQ_DELAY_MS);
    LED_09V_PORT.OUTCLR = LED_09V_PIN_bm;
    LED_05V_PORT.OUTSET = LED_05V_PIN_bm;
    _delay_ms(START_SEQ_DELAY_MS);
}

static void blink_timer_init()
{
    TCB0.CTRLA &= ~(TCB_ENABLE_bm); //Disable Timer B
    TCB0.CTRLA |= TCB_CLKSEL_CLKDIV2_gc;
    TCB0.CTRLB = 0x00;
    TCB0.CTRLB |= TCB_CNTMODE_INT_gc;
    TCB0.CCMP = 0x2000;
    TCB0.INTCTRL |= TCB_CAPT_bm;
}

static void adc_init()
{
    //VREF INIT FOR ADC0
    VREF.CTRLA = 0x00;
    VREF.CTRLA |= VREF_ADC0REFSEL_4V34_gc;

    //ADC0 INIT
    ADC0.CTRLA &= ~(ADC_ENABLE_bm);

    ADC0.CTRLA = 0x00;
    ADC0.CTRLB = 0x00;
    ADC0.CTRLC = 0x00;
    ADC0.CTRLD = 0x00;
    ADC0.CTRLE = 0x00;
    ADC0.SAMPCTRL = 0x00;
    ADC0.CALIB = 0x00;
    ADC0.INTCTRL = 0x00;
    ADC0.MUXPOS = 0x00;

    ADC0.CTRLA |= ADC_RESSEL_10BIT_gc;
    ADC0.CTRLA |= ADC_FREERUN_bm;
    ADC0.CTRLB |= ADC_SAMPNUM_ACC1_gc;
    ADC0.CTRLC |= (ADC_SAMPCAP_bm | ADC_REFSEL_INTREF_gc | ADC_PRESC_DIV16_gc);
    ADC0.CTRLD |= (ADC_INITDLY_DLY256_gc | ADC_ASDV_ASVOFF_gc | ADC_SAMPDLY_gm);
    //Sample Delay = 15 ADC CYC
    ADC0.CTRLE |= ADC_WINCM_OUTSIDE_gc;
    ADC0.SAMPCTRL |= ADC_SAMPLEN_gm; //Sample Length = 31 ADC Cycles
    ADC0.MUXPOS |= VDD_SENSE_LOW_MUXPOS;
    ADC0.INTCTRL |= ADC_WCMP_bm;
    ADC0.CALIB |= ADC_DUTYCYC_DUTY25_gc;
    ADC0.WINLT = 0x00;
    ADC0.WINHT = 0x00;

    ADC0.CTRLA |= ADC_ENABLE_bm;
}

ISR(ADC0_WCOMP_vect)
{
    adc_res_temp = ADC0.RES;

    LED_05V_PORT.OUTCLR = LED_05V_PIN_bm;
    LED_09V_PORT.OUTCLR = LED_09V_PIN_bm;
    LED_12V_PORT.OUTCLR = LED_12V_PIN_bm;
    LED_15V_PORT.OUTCLR = LED_15V_PIN_bm;
    LED_20V_PORT.OUTCLR = LED_20V_PIN_bm;
    LED_28V_PORT.OUTCLR = LED_28V_PIN_bm;
    LED_36V_PORT.OUTCLR = LED_36V_PIN_bm;
    LED_48V_PORT.OUTCLR = LED_48V_PIN_bm;

    BLINK_TIMER_START();
    BLINK_TIMER_CNT_RESET();
    BLINK_TIMER_BLINK_RATE(BLINK_TIMER_CNT_FAST);

    switch (ADC0.MUXPOS)
    {
    case VDD_SENSE_LOW_MUXPOS:
        if (adc_res_temp < winlt_09v_10p_low) //5V Range
        {
            if (adc_res_temp < winlt_05v_05p_low) //5V 10% LOW
            {
                tgl_port = LED_05V_PORT_ADDR;
                tgl_pin_bm = LED_05V_PIN_bm;
                ADC0.WINLT = winlt_05v_10p_low;
                ADC0.WINHT = winht_05v_10p_low;
            }
            else if (adc_res_temp < winlt_05v_norm) //5V 5% LOW
            {
                BLINK_TIMER_BLINK_RATE(BLINK_TIMER_CNT_SLOW);
                tgl_port = LED_05V_PORT_ADDR;
                tgl_pin_bm = LED_05V_PIN_bm;
                ADC0.WINLT = winlt_05v_05p_low;
                ADC0.WINHT = winht_05v_05p_low;
            }
            else if (adc_res_temp < winlt_05v_05p_high) //5V Nominal
            {
                BLINK_TIMER_STOP();
                LED_05V_PORT.OUTSET = LED_05V_PIN_bm;
                ADC0.WINLT = winlt_05v_norm;
                ADC0.WINHT = winht_05v_norm;
            }
            else if (adc_res_temp < winlt_05v_10p_high) //5V 5% HIGH
            {
                BLINK_TIMER_BLINK_RATE(BLINK_TIMER_CNT_SLOW);
                LED_05V_PORT.OUTSET = LED_05V_PIN_bm;
                tgl_port = LED_09V_PORT_ADDR;
                tgl_pin_bm = LED_09V_PIN_bm;
                ADC0.WINLT = winlt_05v_05p_high;
                ADC0.WINHT = winht_05v_05p_high;
            }
            else //5V 10% high
            {
                LED_05V_PORT.OUTSET = LED_05V_PIN_bm;
                tgl_port = LED_09V_PORT_ADDR;
                tgl_pin_bm = LED_09V_PIN_bm;
                ADC0.WINLT = winlt_05v_10p_high;
                ADC0.WINHT = winht_05v_10p_high;
            }
        }
        else if (adc_res_temp < winlt_12v_10p_low) //9V Range
        {
            if (adc_res_temp < winlt_09v_05p_low) //9V 10% LOW
            {
                tgl_port = LED_05V_PORT_ADDR;
                tgl_pin_bm = LED_05V_PIN_bm;
                LED_09V_PORT.OUTSET = LED_09V_PIN_bm;
                ADC0.WINLT = winlt_09v_10p_low;
                ADC0.WINHT = winht_09v_10p_low;
            }
            else if (adc_res_temp < winlt_09v_norm) //9V 5% LOW
            {
                BLINK_TIMER_BLINK_RATE(BLINK_TIMER_CNT_SLOW);
                tgl_port = LED_05V_PORT_ADDR;
                tgl_pin_bm = LED_05V_PIN_bm;
                LED_09V_PORT.OUTSET = LED_09V_PIN_bm;
                ADC0.WINLT = winlt_09v_05p_low;
                ADC0.WINHT = winht_09v_05p_low;
            }
            else if (adc_res_temp < winlt_09v_05p_high) //9V Nominal
            {
                BLINK_TIMER_STOP();
                LED_09V_PORT.OUTSET = LED_09V_PIN_bm;
                ADC0.WINLT = winlt_09v_norm;
                ADC0.WINHT = winht_09v_norm;
            }
            else if (adc_res_temp < winlt_09v_10p_high) //9V 5% HIGH
            {
                BLINK_TIMER_BLINK_RATE(BLINK_TIMER_CNT_SLOW);
                LED_09V_PORT.OUTSET = LED_09V_PIN_bm;
                tgl_port = LED_12V_PORT_ADDR;
                tgl_pin_bm = LED_12V_PIN_bm;
                ADC0.WINLT = winlt_09v_05p_high;
                ADC0.WINHT = winht_09v_05p_high;
            }
            else //9V 10% HIGH
            {
                LED_09V_PORT.OUTSET = LED_09V_PIN_bm;
                tgl_port = LED_12V_PORT_ADDR;
                tgl_pin_bm = LED_12V_PIN_bm;
                ADC0.WINLT = winlt_09v_10p_high;
                ADC0.WINHT = winht_09v_10p_high;
            }
        }
        else if (adc_res_temp < winlt_15v_05p_low) //12 Range
        {
            if (adc_res_temp < winlt_12v_05p_low) //12V 10% LOW
            {
                tgl_port = LED_09V_PORT_ADDR;
                tgl_pin_bm = LED_09V_PIN_bm;
                LED_12V_PORT.OUTSET = LED_12V_PIN_bm;
                ADC0.WINLT = winlt_12v_10p_low;
                ADC0.WINHT = winht_12v_10p_low;
            }
            else if (adc_res_temp < winlt_12v_norm) //12V 5% LOW
            {
                BLINK_TIMER_BLINK_RATE(BLINK_TIMER_CNT_SLOW);
                tgl_port = LED_09V_PORT_ADDR;
                tgl_pin_bm = LED_09V_PIN_bm;
                LED_12V_PORT.OUTSET = LED_12V_PIN_bm;
                ADC0.WINLT = winlt_12v_05p_low;
                ADC0.WINHT = winht_12v_05p_low;
            }
            else if (adc_res_temp < winlt_12v_05p_high) //12V Nominal
            {
                BLINK_TIMER_STOP();
                LED_12V_PORT.OUTSET = LED_12V_PIN_bm;
                ADC0.WINLT = winlt_12v_norm;
                ADC0.WINHT = winht_12v_norm;
            }
            else if (adc_res_temp < winlt_12v_10p_high) //12V 5% HIGH
            {
                BLINK_TIMER_BLINK_RATE(BLINK_TIMER_CNT_SLOW);
                LED_12V_PORT.OUTSET = LED_12V_PIN_bm;
                tgl_port = LED_15V_PORT_ADDR;
                tgl_pin_bm = LED_15V_PIN_bm;
                ADC0.WINLT = winlt_12v_05p_high;
                ADC0.WINHT = winht_12v_05p_high;
            }
            else //12V 10% HIGH
            {
                LED_12V_PORT.OUTSET = LED_12V_PIN_bm;
                tgl_port = LED_15V_PORT_ADDR;
                tgl_pin_bm = LED_15V_PIN_bm;
                ADC0.WINLT = winlt_12v_10p_high;
                ADC0.WINHT = winht_12v_10p_high;
            }
        }
        else if (adc_res_temp < winlt_20v_10p_low) //15V Range
        {
            if (adc_res_temp < winlt_15v_norm) //15V 5% LOW
            {
                BLINK_TIMER_BLINK_RATE(BLINK_TIMER_CNT_SLOW);
                tgl_port = LED_12V_PORT_ADDR;
                tgl_pin_bm = LED_12V_PIN_bm;
                LED_15V_PORT.OUTSET = LED_15V_PIN_bm;
                ADC0.WINLT = winlt_15v_05p_low;
                ADC0.WINHT = winht_15v_05p_low;
            }
            else if (adc_res_temp < winlt_15v_05p_high) //15V Nominal
            {
                BLINK_TIMER_STOP();
                LED_15V_PORT.OUTSET = LED_15V_PIN_bm;
                ADC0.WINLT = winlt_15v_norm;
                ADC0.WINHT = winht_15v_norm;
            }
            else if (adc_res_temp < winlt_15v_10p_high) //15V 5% HIGH
            {
                BLINK_TIMER_BLINK_RATE(BLINK_TIMER_CNT_SLOW);
                LED_15V_PORT.OUTSET = LED_15V_PIN_bm;
                tgl_port = LED_20V_PORT_ADDR;
                tgl_pin_bm = LED_20V_PIN_bm;
                ADC0.WINLT = winlt_15v_05p_high;
                ADC0.WINHT = winht_15v_05p_high;
            }
            else //15V 10% HIGH
            {
                LED_15V_PORT.OUTSET = LED_15V_PIN_bm;
                tgl_port = LED_20V_PORT_ADDR;
                tgl_pin_bm = LED_20V_PIN_bm;
                ADC0.WINLT = winlt_15v_10p_high;
                ADC0.WINHT = winht_15v_10p_high;
            }
        }
        else if (adc_res_temp < winht_20v_05p_high_low_range) //20V Range
        {
            if (adc_res_temp < winlt_20v_05p_low) //20V 10% LOW
            {
                tgl_port = LED_15V_PORT_ADDR;
                tgl_pin_bm = LED_15V_PIN_bm;
                LED_20V_PORT.OUTSET = LED_20V_PIN_bm;
                ADC0.WINLT = winlt_20v_10p_low;
                ADC0.WINHT = winht_20v_10p_low;
            }
            else if (adc_res_temp < winlt_20v_norm) //20V 5% LOW
            {
                BLINK_TIMER_BLINK_RATE(BLINK_TIMER_CNT_SLOW);
                tgl_port = LED_15V_PORT_ADDR;
                tgl_pin_bm = LED_15V_PIN_bm;
                LED_20V_PORT.OUTSET = LED_20V_PIN_bm;
                ADC0.WINLT = winlt_20v_05p_low;
                ADC0.WINHT = winht_20v_05p_low;
            }
            else if (adc_res_temp < winlt_20v_05p_high_low_range) //20V Nominal
            {
                BLINK_TIMER_STOP();
                LED_20V_PORT.OUTSET = LED_20V_PIN_bm;
                ADC0.WINLT = winlt_20v_norm;
                ADC0.WINHT = winht_20v_norm;
            }
            else //20V 5% HIGH low range
            {
                BLINK_TIMER_BLINK_RATE(BLINK_TIMER_CNT_SLOW);
                LED_20V_PORT.OUTSET = LED_20V_PIN_bm;
                tgl_port = LED_28V_PORT_ADDR;
                tgl_pin_bm = LED_28V_PIN_bm;
                ADC0.WINLT = winlt_20v_05p_high_low_range;
                ADC0.WINHT = winht_20v_05p_high_low_range;
            }
        }
        else
        {
            ADC0.MUXPOS = VDD_SENSE_HIGH_MUXPOS;
            ADC0.WINLT = 0x00;
            ADC0.WINHT = 0x00;
        }
        break;
    case VDD_SENSE_HIGH_MUXPOS:
        if (adc_res_temp < winlt_20v_05p_high_high_range)
        {
            ADC0.MUXPOS = VDD_SENSE_LOW_MUXPOS;
            ADC0.WINLT = 0x00;
            ADC0.WINHT = 0x00;
        }
        else if (adc_res_temp < winlt_28v_10p_low) //20V HIGH high range
        {
            if (adc_res_temp < winlt_20v_10p_high) //20V 5% HIGH high range
            {
                BLINK_TIMER_BLINK_RATE(BLINK_TIMER_CNT_SLOW);
                LED_20V_PORT.OUTSET = LED_20V_PIN_bm;
                tgl_port = LED_28V_PORT_ADDR;
                tgl_pin_bm = LED_28V_PIN_bm;
                ADC0.WINLT = winlt_20v_05p_high_high_range;
                ADC0.WINHT = winht_20v_05p_high_high_range;
            }
            else //20V 10% HIGH
            {
                LED_20V_PORT.OUTSET = LED_20V_PIN_bm;
                tgl_port = LED_28V_PORT_ADDR;
                tgl_pin_bm = LED_28V_PIN_bm;
                ADC0.WINLT = winlt_20v_10p_high;
                ADC0.WINHT = winht_20v_10p_high;
            }
        }
        else if (adc_res_temp < winlt_36v_10p_low) //28V range
        {
            if (adc_res_temp < winlt_28v_05p_low) //28V 10% LOW
            {
                LED_28V_PORT.OUTSET = LED_28V_PIN_bm;
                tgl_port = LED_20V_PORT_ADDR;
                tgl_pin_bm = LED_20V_PIN_bm;
                ADC0.WINLT = winlt_28v_10p_low;
                ADC0.WINHT = winht_28v_10p_low;               
            }
            else if (adc_res_temp < winlt_28v_norm) //28V 5% LOW
            {
                BLINK_TIMER_BLINK_RATE(BLINK_TIMER_CNT_SLOW);
                LED_28V_PORT.OUTSET = LED_28V_PIN_bm;
                tgl_port = LED_20V_PORT_ADDR;
                tgl_pin_bm = LED_20V_PIN_bm;
                ADC0.WINLT = winlt_28v_05p_low;
                ADC0.WINHT = winht_28v_05p_low;
            }
            else if (adc_res_temp < winlt_28v_05p_high) //28V NOMINAL
            {
                BLINK_TIMER_STOP();
                LED_28V_PORT.OUTSET = LED_28V_PIN_bm;
                ADC0.WINLT = winlt_28v_norm;
                ADC0.WINHT = winht_28v_norm;
            }
            else if (adc_res_temp < winlt_28v_10p_high) //28V 5% High
            {
                BLINK_TIMER_BLINK_RATE(BLINK_TIMER_CNT_SLOW);
                LED_28V_PORT.OUTSET = LED_28V_PIN_bm;
                tgl_port = LED_36V_PORT_ADDR;
                tgl_pin_bm = LED_36V_PIN_bm;
                ADC0.WINLT = winlt_28v_05p_high;
                ADC0.WINHT = winht_28v_05p_high;
            }
            else
            {
                LED_28V_PORT.OUTSET = LED_28V_PIN_bm;
                tgl_port = LED_36V_PORT_ADDR;
                tgl_pin_bm = LED_36V_PIN_bm;
                ADC0.WINLT = winlt_28v_10p_high;
                ADC0.WINHT = winht_28v_10p_high;                
            }
        }
        else if (adc_res_temp < winlt_48v_10p_low) //36V range
        {
            if (adc_res_temp < winlt_36v_05p_low) //36V 10% LOW
            {
                LED_36V_PORT.OUTSET = LED_36V_PIN_bm;
                tgl_port = LED_28V_PORT_ADDR;
                tgl_pin_bm = LED_28V_PIN_bm;
                ADC0.WINLT = winlt_36v_10p_low;
                ADC0.WINHT = winht_36v_10p_low;                
            }    
            else if (adc_res_temp < winlt_36v_norm) //36V 5% LOW
            {
                BLINK_TIMER_BLINK_RATE(BLINK_TIMER_CNT_SLOW);
                LED_36V_PORT.OUTSET = LED_36V_PIN_bm;
                tgl_port = LED_28V_PORT_ADDR;
                tgl_pin_bm = LED_28V_PIN_bm;
                ADC0.WINLT = winlt_36v_05p_low;
                ADC0.WINHT = winht_36v_05p_low;
            }
            else if (adc_res_temp < winlt_36v_05p_high) //36V NOMINAL
            {
                LED_36V_PORT.OUTSET = LED_36V_PIN_bm;
                BLINK_TIMER_STOP();
                ADC0.WINLT = winlt_36v_norm;
                ADC0.WINHT = winht_36v_norm;
            }
            else if (adc_res_temp < winlt_36v_10p_high)//36V 5% High
            {
                BLINK_TIMER_BLINK_RATE(BLINK_TIMER_CNT_SLOW);
                LED_36V_PORT.OUTSET = LED_36V_PIN_bm;
                tgl_port = LED_48V_PORT_ADDR;
                tgl_pin_bm = LED_48V_PIN_bm;
                ADC0.WINLT = winlt_36v_05p_high;
                ADC0.WINHT = winht_36v_05p_high;
            }
            else //36V 10% High
            {
                LED_36V_PORT.OUTSET = LED_36V_PIN_bm;
                tgl_port = LED_48V_PORT_ADDR;
                tgl_pin_bm = LED_48V_PIN_bm;
                ADC0.WINLT = winlt_36v_10p_high;
                ADC0.WINHT = winht_36v_10p_high;                
            }
        }
        else //48V range
        {
            if (adc_res_temp < winlt_48v_05p_low) //48V 10% LOW
            {
                LED_48V_PORT.OUTSET = LED_48V_PIN_bm;
                tgl_port = LED_36V_PORT_ADDR;
                tgl_pin_bm = LED_36V_PIN_bm;
                ADC0.WINLT = winlt_48v_10p_low;
                ADC0.WINHT = winht_48v_10p_low;               
            }
            else if (adc_res_temp < winlt_48v_norm) //48V 5% LOW
            {
                BLINK_TIMER_BLINK_RATE(BLINK_TIMER_CNT_SLOW);
                LED_48V_PORT.OUTSET = LED_48V_PIN_bm;
                tgl_port = LED_36V_PORT_ADDR;
                tgl_pin_bm = LED_36V_PIN_bm;
                ADC0.WINLT = winlt_48v_05p_low;
                ADC0.WINHT = winht_48v_05p_low;
            }
            else if (adc_res_temp < winlt_48v_05p_high) //48V NOMINAL
            {
                LED_48V_PORT.OUTSET = LED_48V_PIN_bm;
                BLINK_TIMER_STOP();
                ADC0.WINLT = winlt_48v_norm;
                ADC0.WINHT = winht_48v_norm;
            }
            else //48V 5% High
            {
                BLINK_TIMER_BLINK_RATE(BLINK_TIMER_CNT_SLOW);
                tgl_port = LED_48V_PORT_ADDR;
                tgl_pin_bm = LED_48V_PIN_bm;
                ADC0.WINLT = winlt_48v_05p_high;
                ADC0.WINHT = winht_48v_05p_high;
            }
        }
        break;
    default:
        break;
    }

    ADC0.INTFLAGS |= ADC_WCMP_bm;
}

ISR(TCB0_INT_vect)
{
    tgl_port->OUTTGL = tgl_pin_bm;
    TCB0.INTFLAGS |= TCB_CAPT_bm;
}