#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "mongoose.h"


#define AXON_LIGHTS		10
#define AXON_LIGHT_OFFSET	0

#define UD_LIGHTS		30
#define UD_LIGHT_OFFSET		(AXON_LIGHT_OFFSET + AXON_LIGHTS)

#define LD_LIGHTS		10
#define LD_LIGHT_OFFSET		(UD_LIGHT_OFFSET + UD_LIGHTS)

#define N_LIGHTS		(AXON_LIGHTS + UD_LIGHTS + LD_LIGHTS)

#define PHASE_DELAY		1000

static uint8_t red[N_LIGHTS];
static uint8_t green[N_LIGHTS];
static uint8_t blue[N_LIGHTS];
//static uint8_t state[N_LIGHTS];
static int phase[N_LIGHTS];

static void
state_page(struct mg_connection *conn,
	    const struct mg_request_info *ri, void *data)
{
	int i;

	mg_printf(conn, "HTTP/1.1 200 OK\r\n"
		  "content-Type: text/plain\r\n\r\n");

	for (i = 0; i < N_LIGHTS; i++) {
		mg_printf(conn, "%02x: %06x\r\n", i,
			  (red[i] << 16) + (green[i] << 8) + blue[i]);
	}
}

void set(int i)
{
	int state = phase[i] / (0x100 + PHASE_DELAY);
	int idx = phase[i] % (0x100 + PHASE_DELAY);
	if (idx > 0xff)
		idx = 0xff;

	switch (state) {
	case 0:
		red[i] = 0xff;
		green[i] = idx;
		blue[i] = 0x00;
		break;

	case 1:
		red[i] = 0xff - idx;
		green[i] = 0xff;
		blue[i] = 0x00;
		break;

	case 2:
		red[i] = 0x00;
		green[i] = 0xff;
		blue[i] = idx;
		break;

	case 3:
		red[i] = 0x00;
		green[i] = 0xff - idx;
		blue[i] = 0xff;
		break;

	case 4:
		red[i] = idx;
		green[i] = 0x00;
		blue[i] = 0xff;
		break;

	case 5:
		red[i] = 0xff;
		green[i] = 0x00;
		blue[i] = 0xff - idx;
		break;
	}
}

int rot(uint8_t *val, int inc, uint8_t *next_val)
{
	int v = *val + inc;

	if (inc < 0 && v <= 0) {
		*val = 0;
		*next_val += -v;
		return 1;
	}

	if (inc > 0 && v >= 0xff) {
		*val = 0xff;
		*next_val -= v - 0xff;
		return 1;
	}

	*val = v;
	return 0;
}


int main(int argc, char *argv[])
{
	struct mg_context *ctx;
	int i;
	int inc = 8;

	for (i = 0; i < AXON_LIGHTS; i++) {
		phase[i] = (AXON_LIGHTS - i - 1) * 32 + 0x80;
	}

	for (i = 0; i < UD_LIGHTS; i++) {
		phase[i + UD_LIGHT_OFFSET] = 0;
	}

	for (i = 0; i < LD_LIGHTS; i++) {
		phase[i + LD_LIGHT_OFFSET] = 0x180;
	}

	ctx = mg_start();
	mg_set_option(ctx, "ports", "8080");
	mg_set_uri_callback(ctx, "/soma/state", &state_page, NULL);

	for (;;) {
		for (i = 0; i < N_LIGHTS; i++) {
			phase[i] += inc;
			if (phase[i] > (6 * (0x100 + PHASE_DELAY)))
				phase[i] -= (6 * (0x100 + PHASE_DELAY));
			set(i);
		}

		usleep(1000000/100);
	}
}
