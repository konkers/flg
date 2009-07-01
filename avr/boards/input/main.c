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

#include <util.h>
#include <proto.h>

#include <flgmain.h>

#include "pins.h"


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

struct proto_widget widgets[] = {
	PROTO_WIDGET_SWITCH(0),
	PROTO_WIDGET_ADC(0),
	PROTO_WIDGET_ADC(1),
};

struct proto_handlers handlers = {
};


struct proto flg_proto = {
	.handlers = &handlers,
	.widgets = widgets,
	.n_widgets = ARRAY_SIZE(widgets),
};

void flg_recv(uint8_t c)
{
	PORTD |= _BV(D_DATA_LED);
}

void flg_ping(void)
{
	uint8_t sw;

	for(sw = 0; sw < 8; sw++) {
		if (*switches[sw].pinx & _BV(switches[sw].pin))
			proto_switch_set(&widgets[0], sw);
		else
			proto_switch_set(&widgets[0], sw);
	}

	PORTD &= ~_BV(D_DATA_LED);
}

uint16_t adc_sample(uint8_t ch)
{
	ADMUX = ch | _BV(REFS0);
	ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADPS2) | _BV(ADPS1) |_BV(ADPS0);

	while((ADCSRA & _BV(ADSC))) {}

	return ADCL | (ADCH << 8);
}

void flg_set_txen(uint8_t en)
{
	if (en)
		PORTD |= _BV(D_TX_EN);
	else
		PORTD &= ~_BV(D_TX_EN);
}

void flg_hw_setup()
{
	DDRB = 0;
	PORTB = _BV(B_SWITCH0) | _BV(B_SWITCH1) | _BV(B_SWITCH2);

	DDRC = 0;
	PORTC = _BV(C_SWITCH6) | _BV(C_SWITCH7);

	DDRD = _BV(D_TX_EN) | _BV(D_DATA_LED);
	PORTD = _BV(D_SWITCH3) | _BV(D_SWITCH4) | _BV(D_SWITCH5);
}

void flg_work(void)
{
	uint16_t val;

	val = adc_sample(0x6);
	cli();
	proto_adc_set(&widgets[1], val);
	sei();

	val = adc_sample(0x7);
	cli();
	proto_adc_set(&widgets[2], val);
	sei();
}
