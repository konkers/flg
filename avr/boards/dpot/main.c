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

#include <util.h>
#include <proto.h>

#include <flgmain.h>

#include "pins.h"

void handle_dpot(void *data, uint8_t idx, uint8_t val)
{
	if (idx == 0) {
		PORTB &= ~_BV(B_SSEL0);
	} else {
		PORTB &= ~_BV(B_SSEL1);
	}
	SPDR = val;
	while( !(SPSR & _BV(SPIF)) ) {
	}
	val = SPDR;
	if (idx == 0) {
		PORTB |= _BV(B_SSEL0);
	} else {
		PORTB |= _BV(B_SSEL1);
	}
}

struct proto_widget widgets[] = {
	PROTO_WIDGET_DPOT(0),
	PROTO_WIDGET_DPOT(1),
};

struct proto_handlers handlers = {
	.dpot = handle_dpot,
};

struct proto flg_proto = {
	.handlers = &handlers,
	.widgets = widgets,
	.n_widgets = ARRAY_SIZE(widgets),
};

void flg_set_txen(uint8_t en)
{
	if (en)
		PORTD |= _BV(D_TX_EN);
	else
		PORTD &= ~_BV(D_TX_EN);
}

void flg_hw_setup(void)
{
	DDRB = _BV(B_SSEL0) | _BV(B_SSEL1) | _BV(B_MOSI) | _BV(B_SCK);
	PORTB = _BV(B_SSEL0) | _BV(B_SSEL1);

	DDRC = 0;

	DDRD = _BV(D_TX_EN) | _BV(D_DATA_LED);

	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1);//|_BV(CPOL);//|_BV(CPHA);



}

static uint8_t data_led;
void flg_recv(uint8_t c)
{
	PORTD |= _BV(D_DATA_LED);
	data_led = 50;
}


void flg_ping(void)
{
	if (data_led)
		data_led--;
	else
		PORTD &= ~_BV(D_DATA_LED);
}

void flg_work(void)
{
}

