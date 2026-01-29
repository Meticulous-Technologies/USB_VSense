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
/* gpio.h USB Voltage indicator (USB VSense)                                  */
/* ========================================================================== */
/* Description: Main firmware (intended for production)                       */
/*                                                                            */
/* ========================================================================== */
/* Requires:                                                                  */
/*            Part of GPIO module needs gpio.c                                */
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

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef GPIO_H
#define	GPIO_H

#include <avr/io.h>

#define LED_05V_PORT        PORTA
#define LED_05V_PORT_ADDR   0x0400
#define LED_05V_PIN_bm      0x80

#define LED_09V_PORT        PORTA
#define LED_09V_PORT_ADDR   0x0400
#define LED_09V_PIN_bm      0x04

#define LED_12V_PORT        PORTB
#define LED_12V_PORT_ADDR   0x0420
#define LED_12V_PIN_bm      0x10

#define LED_15V_PORT        PORTC
#define LED_15V_PORT_ADDR   0x0440
#define LED_15V_PIN_bm      0x02

#define LED_20V_PORT        PORTB
#define LED_20V_PORT_ADDR   0x0420
#define LED_20V_PIN_bm      0x08

#define LED_28V_PORT        PORTC
#define LED_28V_PORT_ADDR   0x0440
#define LED_28V_PIN_bm      0x01

#define LED_36V_PORT        PORTB
#define LED_36V_PORT_ADDR   0x0420
#define LED_36V_PIN_bm      0x04

#define LED_48V_PORT        PORTB
#define LED_48V_PORT_ADDR   0x0420
#define LED_48V_PIN_bm      0x01

#define VDD_SENSE_LOW_PORT      PORTA
#define VDD_SENSE_LOW_PIN_bm    0x10
#define VDD_SENSE_LOW_PINCTRL   PIN4CTRL

#define VDD_SENSE_HIGH_PORT      PORTA
#define VDD_SENSE_HIGH_PIN_bm    0x20
#define VDD_SENSE_HIGH_PINCTRL   PIN5CTRL

#define VDD_SENSE_LOW_MUXPOS    ADC_MUXPOS_AIN4_gc
#define VDD_SENSE_HIGH_MUXPOS    ADC_MUXPOS_AIN5_gc

#define UNUSED_1_PORT       PORTA
#define UNUSED_1_PIN_bm     0x02
#define UNUSED_1_PINCTRL    PIN1CTRL


#define UNUSED_2_PORT       PORTA
#define UNUSED_2_PIN_bm     0x08
#define UNUSED_2_PINCTRL    PIN3CTRL


#define UNUSED_3_PORT       PORTA
#define UNUSED_3_PIN_bm     0x40
#define UNUSED_3_PINCTRL    PIN6CTRL

#define UNUSED_4_PORT       PORTB
#define UNUSED_4_PIN_bm     0x02
#define UNUSED_4_PINCTRL    PIN1CTRL

#define UNUSED_5_PORT       PORTB
#define UNUSED_5_PIN_bm     0x20
#define UNUSED_5_PINCTRL    PIN5CTRL

#define UNUSED_6_PORT       PORTC
#define UNUSED_6_PIN_bm     0x04
#define UNUSED_6_PINCTRL    PIN2CTRL

#define UNUSED_7_PORT      PORTC
#define UNUSED_7_PIN_bm    0x08
#define UNUSED_7_PINCTRL   PIN3CTRL

void gpio_init(void);

#endif	/* GPIO_H */

