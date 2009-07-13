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

#ifndef __proto_h__
#define __proto_h__

#ifdef __cplusplus
extern "C" {
#endif
#if 0
} /* stupid trick to balance out above */
#endif

#include <stdint.h>

#define PROTO_SOF	0x53
#define PROTO_EOF	0x45

enum proto_widget_type {
	PROTO_WIDGET_TYPE_RELAY,
	PROTO_WIDGET_TYPE_LIGHT,
	PROTO_WIDGET_TYPE_SWITCH,
	PROTO_WIDGET_TYPE_ADC,
};

struct proto_widget {
	uint8_t	type;
	uint8_t	idx;
	union {
		struct {
			uint8_t	timeout;
			uint8_t	counter;
		} relay;
		struct {
			uint8_t	state;
		} sw;
		struct {
			uint16_t val;
		} adc;
	};
} __attribute__((packed));

/*
 * these do no used named element initializaion as to make them
 * compatible with C++.  Care must be taken to align them with
 * the struct above
 */
#define PROTO_WIDGET_RELAY(idx, timeout) \
	{ PROTO_WIDGET_TYPE_RELAY, (idx), {{(timeout), 0}} }
#define PROTO_WIDGET_LIGHT(idx)	\
	{ PROTO_WIDGET_TYPE_LIGHT, (idx) }
#define PROTO_WIDGET_SWITCH(idx)	\
	{ PROTO_WIDGET_TYPE_SWITCH, (idx), {{0}} }
#define PROTO_WIDGET_ADC(idx)		\
	{ PROTO_WIDGET_TYPE_ADC, (idx), {{0}} }


struct proto_packet {
	uint8_t sof;
	uint8_t	addr;
	uint8_t	cmd;
	uint8_t	val;
	uint8_t	crc;
	uint8_t eof;
} __attribute__((packed));

struct proto_handlers {
	void (* relay)(void *data, uint8_t idx, uint8_t state);
	void (* light)(void *data, uint8_t idx, uint8_t val);
	void (* set_addr)(void *data, uint8_t addr);

	void (* send)(void *data, uint8_t *pkt_data, int len);
	void (* resp)(void *data, struct proto_packet *pkt);
};

enum proto_cmd {
	PROTO_CMD_RELAY_SET =		0x00,
	PROTO_CMD_RELAY_CLEAR =		0x01,

	PROTO_CMD_LIGHT0_SET =		0x20,
	PROTO_CMD_LIGHT1_SET =		0x21,
	PROTO_CMD_LIGHT2_SET =		0x22,
	PROTO_CMD_LIGHT3_SET =		0x23,
	PROTO_CMD_LIGHT4_SET =		0x24,
	PROTO_CMD_LIGHT5_SET =		0x25,
	PROTO_CMD_LIGHT6_SET =		0x26,
	PROTO_CMD_LIGHT7_SET =		0x27,
	PROTO_CMD_LIGHT8_SET =		0x28,
	PROTO_CMD_LIGHT9_SET =		0x29,
	PROTO_CMD_LIGHTA_SET =		0x2a,
	PROTO_CMD_LIGHTB_SET =		0x2b,
	PROTO_CMD_LIGHTC_SET =		0x2c,
	PROTO_CMD_LIGHTD_SET =		0x2d,
	PROTO_CMD_LIGHTE_SET =		0x2e,
	PROTO_CMD_LIGHTF_SET =		0x2f,

	PROTO_CMD_SWITCH_QUERY=		0x30,
	PROTO_CMD_ADC_QUERY_LO=		0x38,
	PROTO_CMD_ADC_QUERY_HI=		0x39,

	PROTO_CMD_SET_ADDR=		0x40,

	PROTO_CMD_GET_STATUS=		0x50,
};

enum proto_state {
	PROTO_STATE_IDLE,
	PROTO_STATE_ADDR,
	PROTO_STATE_CMD,
	PROTO_STATE_VAL,
	PROTO_STATE_CRC,
	PROTO_STATE_EOF,
};

struct proto {
	void			*handler_data;
	struct proto_handlers	*handlers;
	struct proto_widget	*widgets;
	uint8_t			n_widgets;
	uint8_t			status;

	struct proto_packet	packet;
	enum proto_state	state;
	uint8_t addr;
	uint8_t crc;
};

void proto_init(struct proto *p, uint8_t addr);
void proto_packet_seal(struct proto_packet *packet);
void proto_recv(struct proto *p, uint8_t proto);
void proto_ping(struct proto *p);

static inline void proto_switch_set(struct proto_widget *w, int sw)
{
	w->sw.state |= 1 << (sw & 0x7);
}

static inline void proto_switch_clear(struct proto_widget *w, int sw)
{
	w->sw.state &= ~(1 << (sw & 0x7));
}

static inline void proto_adc_set(struct proto_widget *w, uint16_t val)
{
	w->adc.val = val;
}

static inline void proto_set_status(struct proto *p, uint8_t status)
{
	p->status = status;
}


#if 0
{ /* stupid trick to balance out below */
#endif
#ifdef __cplusplus
}
#endif

#endif /* __proto_h__ */


