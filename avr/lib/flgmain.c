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

	flg_set_txen(1);

	uart_enable_udre();
	uart_write_dr(*pkt_data);
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
	if (pkt_len > 0) {
		uart_write_dr(*pkt);
	} else {
		flg_set_txen(0);
		uart_disable_udre();
	}
}

#ifdef NEW_UART
ISR( USART_RX_vect )
#else
ISR( USART_RXC_vect )
#endif
{
	uint8_t c = uart_read_dr();
	proto_recv(&flg_proto, c);
	flg_recv(c);
}

#ifdef TIMER0_COMPA_vect
ISR( TIMER0_COMPA_vect )
#else
ISR( TIMER2_COMP_vect )
#endif
{
	wdt_reset();
	proto_ping(&flg_proto);
	flg_ping();
}

static void set_timer2(uint8_t cs, uint8_t oc)
{
#ifdef TCCR0A
	TCCR0A = cs;
	/* set TC into CTC mode */
	TCCR0A = _BV(WGM01);
	TIMSK2 = _BV(OCIE0A);
	OCR0A = oc;
#else
	TCCR2 = cs;
	OCR2 = oc;
	TIMSK |= _BV(OCIE2);
#endif
}


int main( void )
{
	cli();

	flg_pin_setup();
#if (FOSC == 18432000UL)
	/*
	 * Fosc = 18432000
	 * Fping = 100 Hz
	 *
	 * Fosc / Fping / 1024 = 180;
	 */
	set_timer2(_BV(CS22) | _BV(CS21) | _BV(CS20), 180);
#elif (FOSC == 7372800UL)
	/*
	 * Fosc = 7372800
	 * Fping = 100 Hz
	 *
	 * Fosc / Fping / 1024 = 72;
	 */
	set_timer2(_BV(CS22) | _BV(CS21) | _BV(CS20), 72);
#else
#error "FLGMAIN: unsupported FOSC freq"
#endif
	config_init();
	uart_init(uart_baud(FOSC, 115200));

	flg_proto.handlers->set_addr = &flg_set_addr;
	flg_proto.handlers->send = &flg_queue_pkt;
	proto_init(&flg_proto, config.addr);

	sei();

	wdt_enable(WDTO_1S);

	while (1) {
		flg_work();
	}
}

#endif /* FLGMAIN */
