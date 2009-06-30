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
#include <avr/wdt.h>

#include <uart.h>
#include <config.h>

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
};

void handle_relay(void *data, uint8_t idx, uint8_t state)
{
	if (state)
		*relays[idx].port |= _BV(relays[idx].pin);
	else
		*relays[idx].port &= ~_BV(relays[idx].pin);
}

void set_addr(void *data, uint8_t addr)
{
	config.addr = addr;
	config_write();
	cli();
	wdt_disable();
	wdt_enable(WDTO_15MS);
	while(1) {}
}

struct proto_widget widgets[] = {
	PROTO_WIDGET_RELAY(0,10),
	PROTO_WIDGET_RELAY(1,10),
	PROTO_WIDGET_RELAY(2,10),
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
	wdt_reset();
	proto_ping(&p);
	PORTD &= ~_BV(D_DATA_LED);
}

int main( void )
{
	cli();

	DDRB =  _BV(B_RELAY0) | _BV(B_RELAY1) | _BV(B_RELAY2);

	DDRC = 0;

	DDRD = _BV(D_TX_EN) | _BV(D_DATA_LED);

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

	config_init();
	uart_init(uart_baud(FOSC, 115200));
	proto_init(&p, config.addr);

	sei();

	wdt_enable(WDTO_1S);

	while (1) {
	}
}
