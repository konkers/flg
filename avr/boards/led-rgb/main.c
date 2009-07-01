/*
 * Copyright 2009 Erik Gilling
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include <uart.h>

#include <util.h>
#include <proto.h>

#include "pins.h"

uint8_t led_val[3];
uint8_t bam_mask;

void handle_led(void *data, uint8_t idx, uint8_t val)
{
	led_val[idx] = val;
}


struct proto_widget widgets[] = {
	PROTO_WIDGET_LIGHT(0),
	PROTO_WIDGET_LIGHT(1),
	PROTO_WIDGET_LIGHT(2),
};

struct proto_handlers handlers = {
	.relay = &handle_led,
};


struct proto flg_proto = {
	.handlers = &handlers,
	.widgets = widgets,
	.n_widgets = ARRAY_SIZE(widgets),
};

void flg_recv(uint8_t c)
{
	PORTD |= _BV(D_DATA_LED);
}

void flg_ping(void)
{
	PORTD &= ~_BV(D_DATA_LED);
}

ISR( TIMER2_COMPA_vect )
{
	uint8_t port;

	/* we have to do this instead of a rol becaulse the
	 * AVR rol rotates *TROUGH* carry.
	 */
	bam_mask <<= 1;
	if(!bam_mask)
		bam_mask= 0x01;

	OCR2A = bam_mask;

	port = PORTC;

	if (led_val[0] & bam_mask)
		port |= _BV(C_RED);
	else
		port &= ~_BV(C_RED);

	if (led_val[1] & bam_mask)
		port |= _BV(C_GREEN);
	else
		port &= ~_BV(C_GREEN);

	if (led_val[2] & bam_mask)
		port |= _BV(C_BLUE);
	else
		port &= ~_BV(C_BLUE);

	PORTC = port;
}

void flg_set_txen(uint8_t en)
{
}

void flg_hw_setup(void)
{
	DDRB = 0;

	DDRC = _BV(C_RED) | _BV(C_GREEN) | _BV(C_BLUE) |
		_BV(C_RED_OD) | _BV(C_GREEN_OD) | _BV(C_BLUE_OD);

	DDRD = _BV(D_DATA_LED) | _BV(D_GREEN_OD2) | _BV(D_BLUE_OD2);

	bam_mask = 0x01;
	OCR2A = bam_mask;

	/* set TC into CTC mode */
	TCCR2A = _BV(WGM21);
	/* enable output compare interrupt */
	TIMSK2 = _BV(OCIE2A);
	/* use 64 prescalar giving us a minimum frequency of 244Hz */
	TCCR2B = _BV(CS21) | _BV(CS20);
}

void flg_work(void)
{
}
