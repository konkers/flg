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

#ifndef __libavr_uart_h__
#define __libavr_uart_h__

#include <avr/pgmspace.h>
#include <avr/io.h>

#ifdef UBRR0H
#define NEW_UART
#endif

static inline uint16_t uart_baud(uint32_t fosc, uint32_t baud)
{
	return fosc / 16 / baud - 1;
}

static inline uint8_t uart_read_dr(void)
{
#ifdef NEW_UART
	return UDR0;
#else
	return UDR;
#endif
}

static inline void uart_write_dr(uint8_t d)
{
#ifdef NEW_UART
	UDR0 = d;
#else
	UDR = d;
#endif
}

static inline void uart_enable_udre(void)
{
#ifdef NEW_UART
	UCSR0B |= _BV(UDRE0);
#else
	UCSRB |= _BV(UDRE);
#endif
}

static inline void uart_disable_udre(void)
{
#ifdef NEW_UART
	UCSR0B &= ~_BV(UDRE0);
#else
	UCSRB &= ~_BV(UDRE);
#endif
}

static inline void uart_enable_tx(void)
{
#ifdef NEW_UART
	UCSR0B |= _BV(TXCIE0);
#else
	UCSRB |= _BV(TXCIE);
#endif
}

static inline void uart_disable_tx(void)
{
#ifdef NEW_UART
	UCSR0B &= ~_BV(TXCIE0);
#else
	UCSRB &= ~_BV(TXCIE);
#endif
}

void uart_init(uint16_t ubrr);
void uart_putc(unsigned char data);
void uart_puts(const char *s);
void uart_puts_P(const prog_char *s);
void uart_printhex(uint8_t val);
int uart_has_data(void);
unsigned char uart_poll_getchar(void);

#endif /* __libavr_uart_h__ */
