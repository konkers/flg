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

#define FOSC 18432000UL

struct relay_cfg {
	volatile uint8_t *port;
	uint8_t pin;
} __attribute__((packed));

struct relay_cfg relays[] = {
	{&PORTB, B_RELAY0},
	{&PORTB, B_RELAY1},
	{&PORTB, B_RELAY2},
	{&PORTD, D_RELAY3},
	{&PORTD, D_RELAY4},
	{&PORTD, D_RELAY5},
	{&PORTC, C_RELAY6},
	{&PORTC, C_RELAY7},
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

struct proto p = {
	.handlers = &handlers,
	.widgets = widgets,
	.n_widgets = ARRAY_SIZE(widgets),
};

ISR( USART_RX_vect )
{
	proto_recv(&p, UDR0);
	PORTD |= _BV(D_DATA_LED);
}

ISR( TIMER0_COMPA_vect )
{
	proto_ping(&p);
	PORTD &= ~_BV(D_DATA_LED);
}

int main( void )
{
	uint8_t addr;

	cli();

	DDRB =  _BV(B_RELAY0) | _BV(B_RELAY1) | _BV(B_RELAY2);
	PORTB = _BV(B_ADDR5) | _BV(B_ADDR6) | _BV(B_ADDR7);

	DDRC = _BV(C_RELAY6) | _BV(C_RELAY7);
	PORTC = _BV(C_ADDR0) | _BV(C_ADDR1) | _BV(C_ADDR2) | _BV(C_ADDR3);

	DDRD = _BV(D_TX_EN) | _BV(D_DATA_LED) |
		_BV(D_RELAY3) | _BV(D_RELAY4) | _BV(D_RELAY5);
	PORTD = _BV(D_ADDR4);

	/*
	 * Fosc = 18432000
	 * Fping = 100 Hz
	 *
	 * Fosc / Fping / 1024 = 180;
	 */
	TCCR0A = _BV(CS22) | _BV(CS21) | _BV(CS20);
	/* set TC into CTC mode */
	TCCR0A = _BV(WGM01);
	TIMSK2 = _BV(OCIE0A);
	OCR0A = 180;

	addr  = ~PINC & 0xf;
	addr |= ~PIND & _BV(D_ADDR4);
	addr |= (~PIND << 2) & 0xe0;

	uart_init(uart_baud(FOSC, 115200));
	proto_init(&p, addr);

	sei();

	while (1) {
	}
}
