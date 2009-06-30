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

uint8_t *pkt;
uint8_t pkt_len;

struct switch_cfg {
	volatile uint8_t *pinx;
	uint8_t pin;
} __attribute__((packed));

struct switch_cfg switches[] = {
	{&PINB, B_SWITCH0},
	{&PINB, B_SWITCH1},
	{&PINB, B_SWITCH2},
	{&PIND, D_SWITCH3},
	{&PIND, D_SWITCH4},
	{&PIND, D_SWITCH5},
	{&PINC, C_SWITCH6},
	{&PINC, C_SWITCH7},
};

void queue_pkt(void *data, uint8_t *pkt_data, int len)
{
	if (pkt_len > 0)
		return;


	pkt = pkt_data;
	pkt_len = len;

	UCSR0B |= _BV(UDRE0);

	UDR0 = *pkt_data;
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
	PROTO_WIDGET_SWITCH(0),
};

struct proto_handlers handlers = {
	.set_addr = &set_addr,
	.send = &queue_pkt,
};


struct proto p = {
	.handlers = &handlers,
	.widgets = widgets,
	.n_widgets = ARRAY_SIZE(widgets),
};

ISR( USART_UDRE_vect )
{
	pkt++;
	pkt_len--;
	if (pkt_len > 0)
		UDR0 = *pkt;
	else
		UCSR0B &= _BV(UDRE0);
}


ISR( USART_RX_vect )
{
	proto_recv(&p, UDR0);
	PORTD |= _BV(D_DATA_LED);
}

ISR( TIMER0_COMPA_vect )
{
	uint8_t sw;

	wdt_reset();
	proto_ping(&p);

	for(sw = 0; sw < 8; sw++) {
		if (*switches[sw].pinx & _BV(switches[sw].pin))
			proto_switch_set(&widgets[0], sw);
		else
			proto_switch_set(&widgets[0], sw);
	}
	PORTD &= ~_BV(D_DATA_LED);
}

int main( void )
{
	cli();

	DDRB = 0;
	PORTB = _BV(B_SWITCH0) | _BV(B_SWITCH1) | _BV(B_SWITCH2);

	DDRC = 0;
	PORTC = _BV(C_SWITCH6) | _BV(C_SWITCH7);

	DDRD = _BV(D_TX_EN) | _BV(D_DATA_LED);
	PORTD = _BV(D_SWITCH3) | _BV(D_SWITCH4) | _BV(D_SWITCH5);

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
