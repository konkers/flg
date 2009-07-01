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

struct relay_cfg {
	volatile uint8_t *port;
	uint8_t pin;
} __attribute__((packed));

struct relay_cfg relays[] = {
	{&PORTB, B_RELAY0},
	{&PORTB, B_RELAY1},
	{&PORTB, B_RELAY2},
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
};

struct proto_handlers handlers = {
	.relay = &handle_relay,
	.set_addr = &flg_set_addr,
};

struct proto flg_proto = {
	.handlers = &handlers,
	.widgets = widgets,
	.n_widgets = ARRAY_SIZE(widgets),
};

void flg_pin_setup(void)
{
	DDRB =  _BV(B_RELAY0) | _BV(B_RELAY1) | _BV(B_RELAY2);

	DDRC = 0;

	DDRD = _BV(D_TX_EN) | _BV(D_DATA_LED);
}

void flg_recv(uint8_t c)
{
	PORTD |= _BV(D_DATA_LED);
}

void flg_ping(void)
{
	PORTD &= ~_BV(D_DATA_LED);
}

void flg_work(void)
{
}

