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

#include <avr/interrupt.h>
#include <avr/wdt.h>

#include <proto.h>
#include <util.h>

#include <uart.h>
#include <config.h>
#include <flgmain.h>

#ifdef FLGMAIN

#ifndef FOSC
#error "FLGMAIN: FOSC not defined"
#endif

static uint8_t *pkt;
static uint8_t pkt_len;


void flg_queue_pkt(void *data, uint8_t *pkt_data, int len)
{
	if (pkt_len > 0)
		return;

	pkt = pkt_data;
	pkt_len = len;

	UCSR0B |= _BV(UDRE0);

	UDR0 = *pkt_data;
}

void flg_set_addr(void *data, uint8_t addr)
{
	config.addr = addr;
	config_write();
	cli();
	wdt_disable();
	wdt_enable(WDTO_15MS);
	while(1) {}
}


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
	uint8_t c = UDR0;
	proto_recv(&flg_proto, c);
	flg_recv(c);
}

ISR( TIMER0_COMPA_vect )
{
	wdt_reset();
	proto_ping(&flg_proto);
	flg_ping();
}



int main( void )
{
	cli();

	flg_pin_setup();
	/*
	 * Fosc = 18432000
	 * Fping = 100 Hz
	 *
	 * Fosc / Fping / 1024 = 180;
	 */
#if (FOSC == 18432000UL)
	TCCR0A = _BV(CS22) | _BV(CS21) | _BV(CS20);
	/* set TC into CTC mode */
	TCCR0A = _BV(WGM01);
	TIMSK2 = _BV(OCIE0A);
	OCR0A = 180;
#else
#error "FLGMAIN: unsupported FOSC freq"
#endif
	config_init();
	uart_init(uart_baud(FOSC, 115200));
	proto_init(&flg_proto, config.addr);

	sei();

	wdt_enable(WDTO_1S);

	while (1) {
		flg_work();
	}
}

#endif /* FLGMAIN */
