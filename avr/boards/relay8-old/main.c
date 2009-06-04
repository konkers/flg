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

#define FOSC 7372800UL


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

void handle_relay(uint8_t idx, uint8_t state)
{
	if (state)
		*relays[idx].port |= _BV(relays[idx].pin);
	else
		*relays[idx].port &= ~_BV(relays[idx].pin);
}


struct proto_widget widgets[] = {
	{ .type = PROTO_WIDGET_RELAY, .idx = 0, .timeout = 10 },
	{ .type = PROTO_WIDGET_RELAY, .idx = 1, .timeout = 10 },
	{ .type = PROTO_WIDGET_RELAY, .idx = 2, .timeout = 10 },
	{ .type = PROTO_WIDGET_RELAY, .idx = 3, .timeout = 10 },
	{ .type = PROTO_WIDGET_RELAY, .idx = 4, .timeout = 10 },
	{ .type = PROTO_WIDGET_RELAY, .idx = 5, .timeout = 10 },
	{ .type = PROTO_WIDGET_RELAY, .idx = 6, .timeout = 10 },
	{ .type = PROTO_WIDGET_RELAY, .idx = 7, .timeout = 10 },
};

struct proto_handlers handlers = {
	.relay = &handle_relay,
};


struct proto p = {
	.handlers = &handlers,
	.widgets = widgets,
	.n_widgets = ARRAY_SIZE(widgets),
};

ISR( USART_RXC_vect )
{
	proto_recv(&p, UDR);
	PORTB |= _BV(DATALED);
}

ISR( TIMER2_COMP_vect )
{
	proto_ping(&p);
	PORTB &= ~_BV(DATALED);
}

int main( void )
{
	uint8_t addr;

	cli();

	DDRB = _BV(DATALED) | _BV(RELAY6) | _BV(RELAY7);
	PORTB = 0;

	DDRC = _BV(RELAY0) | _BV(RELAY1) |
		_BV(RELAY2) | _BV(RELAY3) |
		_BV(RELAY4) | _BV(RELAY5);
	PORTC = 0;

	DDRD = (1 << RS485TRANSMIT);
	/* enable pullups on switches */
	PORTD = _BV(SWITCH1) |
		_BV(SWITCH2) |
		_BV(SWITCH3) |
		_BV(SWITCH4) |
		_BV(SWITCH5);

	/*
	 * Fosc = 7372800
	 * Fping = 100 Hz
	 *
	 * Fosc / Fping / 1024 = 72;
	 */
	TCCR2 = _BV(CS22) | _BV(CS21) | _BV(CS20);
	OCR2 = 72;

	addr = (~(PIND >> SWITCH_SHIFT)) & SWITCH_MASK;

	uart_init(uart_baud(FOSC, 115200));
	proto_init(&p, addr);

	sei();

	while (1) {
	}
}
