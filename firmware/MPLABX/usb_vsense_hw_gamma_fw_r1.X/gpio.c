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
/* gpio.c USB Voltage indicator (USB VSense)                                  */
/* ========================================================================== */
/* Description: Main firmware (intended for production)                       */
/*                                                                            */
/* ========================================================================== */
/* Requires:                                                                  */
/*            Part of GPIO module needs gpio.h                                */
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

#include "gpio.h"
#include <avr/io.h>
#include <xc.h>

void gpio_init()
{
    LED_05V_PORT.DIRSET = LED_05V_PIN_bm;
    LED_05V_PORT.OUTCLR = LED_05V_PIN_bm;
    
    LED_09V_PORT.DIRSET = LED_09V_PIN_bm;
    LED_09V_PORT.OUTCLR = LED_09V_PIN_bm;
    
    LED_12V_PORT.DIRSET = LED_12V_PIN_bm;
    LED_12V_PORT.OUTCLR = LED_12V_PIN_bm;
    
    LED_15V_PORT.DIRSET = LED_15V_PIN_bm;
    LED_15V_PORT.OUTCLR = LED_15V_PIN_bm;
    
    LED_20V_PORT.DIRSET = LED_20V_PIN_bm;
    LED_20V_PORT.OUTCLR = LED_20V_PIN_bm;
    
    LED_28V_PORT.DIRSET = LED_28V_PIN_bm;
    LED_28V_PORT.OUTCLR = LED_28V_PIN_bm;
    
    LED_36V_PORT.DIRSET = LED_36V_PIN_bm;
    LED_36V_PORT.OUTCLR = LED_36V_PIN_bm;
    
    LED_48V_PORT.DIRSET = LED_48V_PIN_bm;
    LED_48V_PORT.OUTCLR = LED_48V_PIN_bm;
    
    VDD_SENSE_LOW_PORT.DIRCLR = VDD_SENSE_LOW_PIN_bm;
    VDD_SENSE_LOW_PORT.VDD_SENSE_LOW_PINCTRL |= PORT_ISC_INPUT_DISABLE_gc;
    
    VDD_SENSE_HIGH_PORT.DIRCLR = VDD_SENSE_HIGH_PIN_bm;
    VDD_SENSE_HIGH_PORT.VDD_SENSE_HIGH_PINCTRL |= PORT_ISC_INPUT_DISABLE_gc;
    
    UNUSED_1_PORT.DIRCLR = UNUSED_1_PIN_bm;
    UNUSED_1_PORT.UNUSED_1_PINCTRL |= PORT_PULLUPEN_bm;
    
    UNUSED_2_PORT.DIRCLR = UNUSED_2_PIN_bm;
    UNUSED_2_PORT.UNUSED_2_PINCTRL |= PORT_PULLUPEN_bm;

    UNUSED_3_PORT.DIRCLR = UNUSED_3_PIN_bm;
    UNUSED_3_PORT.UNUSED_3_PINCTRL |= PORT_PULLUPEN_bm;
   
    UNUSED_4_PORT.DIRCLR = UNUSED_4_PIN_bm;
    UNUSED_4_PORT.UNUSED_4_PINCTRL |= PORT_PULLUPEN_bm;
    
    UNUSED_5_PORT.DIRCLR = UNUSED_5_PIN_bm;
    UNUSED_5_PORT.UNUSED_5_PINCTRL |= PORT_PULLUPEN_bm;
    
    UNUSED_6_PORT.DIRCLR = UNUSED_6_PIN_bm;
    UNUSED_6_PORT.UNUSED_6_PINCTRL |= PORT_PULLUPEN_bm;
   
    UNUSED_7_PORT.DIRCLR = UNUSED_7_PIN_bm;
    UNUSED_7_PORT.UNUSED_7_PINCTRL |= PORT_PULLUPEN_bm;
}

