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

#include <util.h>
#include <proto.h>

#include <flgmain.h>

#include "pins.h"

struct relay_cfg {
	volatile uint8_t *port;
	uint8_t pin;
} __attribute__((packed));

struct relay_cfg relays[] = {
	{&PORTC, RELAY0},
	{&PORTC, RELAY1},
	{&PORTC, RELAY2},
	{&PORTC, RELAY3},
	{&PORTC, RELAY4},
	{&PORTC, RELAY5},
	{&PORTB, RELAY6},
	{&PORTB, RELAY7},
};

void handle_relay(void *data, uint8_t idx, uint8_t state)
{
	if (state)
		*relays[idx].port |= _BV(relays[idx].pin);
	else
		*relays[idx].port &= ~_BV(relays[idx].pin);
}


struct proto_widget widgets[] = {
	PROTO_WIDGET_RELAY(0,10),
	PROTO_WIDGET_RELAY(1,10),
	PROTO_WIDGET_RELAY(2,10),
	PROTO_WIDGET_RELAY(3,10),
	PROTO_WIDGET_RELAY(4,10),
	PROTO_WIDGET_RELAY(5,10),
	PROTO_WIDGET_RELAY(6,10),
	PROTO_WIDGET_RELAY(7,10),
};

struct proto_handlers handlers = {
	.relay = &handle_relay,
};


struct proto flg_proto = {
	.handlers = &handlers,
	.widgets = widgets,
	.n_widgets = ARRAY_SIZE(widgets),
};

void flg_recv(uint8_t c)
{
	PORTB |= _BV(DATALED);
}

void flg_ping(void)
{
	PORTB &= ~_BV(DATALED);
}

void flg_set_txen(uint8_t en)
{
	if (en)
		PORTD |= _BV(RS485TRANSMIT);
	else
		PORTD &= ~_BV(RS485TRANSMIT);
}

void flg_hw_setup(void)
{
	DDRB = _BV(DATALED) | _BV(RELAY6) | _BV(RELAY7);
	PORTB = _BV(SWITCH6) | _BV(SWITCH7) | _BV(SWITCH8);

	DDRC = _BV(RELAY0) | _BV(RELAY1) |
		_BV(RELAY2) | _BV(RELAY3) |
		_BV(RELAY4) | _BV(RELAY5);
	PORTC = 0;

	DDRD = _BV(RS485TRANSMIT);
	/* enable pullups on switches */
	PORTD = _BV(SWITCH1) |
		_BV(SWITCH2) |
		_BV(SWITCH3) |
		_BV(SWITCH4) |
		_BV(SWITCH5);
}

void flg_work(void)
{
}
