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

uint8_t led_val[2];
uint8_t bam_mask;

void handle_led(void *data, uint8_t idx, uint8_t val)
{
	led_val[idx] = val;
}


struct proto_widget widgets[] = {
	{ .type = PROTO_WIDGET_LIGHT, .idx = 0 },
	{ .type = PROTO_WIDGET_LIGHT, .idx = 1 },
	{ .type = PROTO_WIDGET_LIGHT, .idx = 2 },
};

struct proto_handlers handlers = {
	.relay = &handle_led,
};


struct proto p = {
	.handlers = &handlers,
	.widgets = widgets,
	.n_widgets = ARRAY_SIZE(widgets),
};

ISR( USART_RX_vect )
{
	proto_recv(&p, UDR0);
	PORTB |= _BV(B_DATA_LED);
}

ISR( TIMER0_COMPA_vect )
{
	proto_ping(&p);
	PORTB &= ~_BV(B_DATA_LED);
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

	port = PORTB;

	if (led_val[0] & bam_mask)
		port |= _BV(B_LED_R);
	else
		port &= ~_BV(B_LED_R);

	if (led_val[1] & bam_mask)
		port |= _BV(B_LED_G);
	else
		port &= ~_BV(B_LED_G);

	PORTB = port;

	if (led_val[2] & bam_mask)
		PORTD |= _BV(D_LED_B);
	else
		PORTD &= ~_BV(D_LED_B);
}

int main( void )
{
	uint8_t addr;

	cli();

	DDRB = _BV(B_DATA_LED) | _BV(B_LED_R) | _BV(B_LED_G);
	PORTB = _BV(B_ADDR5) | _BV(B_ADDR6) | _BV(B_ADDR7);

	DDRC = 0;
	PORTC = _BV(C_ADDR0) | _BV(C_ADDR1) | _BV(C_ADDR2) | _BV(C_ADDR3);

	DDRD = _BV(D_TX_EN) | _BV(D_LED_B);
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

	bam_mask = 0x01;
	OCR2A = bam_mask;

	/* set TC into CTC mode */
	TCCR2A = _BV(WGM21);
	/* enable output compare interrupt */
	TIMSK2 = _BV(OCIE2A);
	/* use 64 prescalar giving us a minimum frequency of 244Hz */
	TCCR2B = _BV(CS21) | _BV(CS20);



	addr  = ~PINC & 0xf;
	addr |= ~PIND & _BV(D_ADDR4);
	addr |= (~PIND << 2) & 0xe0;

	uart_init(uart_baud(FOSC, 115200));
	proto_init(&p, addr);

	sei();

	while (1) {
	}
}
