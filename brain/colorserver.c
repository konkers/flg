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

static uint8_t red[N_LIGHTS];
static uint8_t green[N_LIGHTS];
static uint8_t blue[N_LIGHTS];
static uint8_t state[N_LIGHTS];

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
		state[i] = ((AXON_LIGHTS - 1) - i) % 6;
		switch (state[i]) {
		case 0:
			red[i] = 0xff;
			green[i] = 0x00;
			blue[i] = 0x00;
			break;

		case 1:
			red[i] = 0xff;
			green[i] = 0xff;
			blue[i] = 0x00;
			break;

		case 2:
			red[i] = 0x00;
			green[i] = 0xff;
			blue[i] = 0x00;
			break;

		case 3:
			red[i] = 0x00;
			green[i] = 0xff;
			blue[i] = 0xff;
			break;

		case 4:
			red[i] = 0x00;
			green[i] = 0x00;
			blue[i] = 0xff;
			break;

		case 5:
			red[i] = 0xff;
			green[i] = 0x00;
			blue[i] = 0xff;
			break;
		}
	}

	ctx = mg_start();
	mg_set_option(ctx, "ports", "8080");
	mg_set_uri_callback(ctx, "/soma/state", &state_page, NULL);

	for (;;) {
		for (i = 0; i < AXON_LIGHTS; i++) {
			switch (state[i]) {
			case 0:
				if (rot(&green[i], inc, &red[i]))
					state[i]++;
				break;

			case 1:
				if (rot(&red[i], -inc, &blue[i]))
					state[i]++;
				break;

			case 2:
				if (rot(&blue[i], inc, &green[i]))
					state[i]++;
				break;

			case 3:
				if (rot(&green[i], -inc, &red[i]))
					state[i]++;
				break;

			case 4:
				if (rot(&red[i], inc, &blue[i]))
					state[i]++;
				break;

			case 5:
				if (rot(&blue[i], -inc, &green[i]))
					state[i] = 0;
				break;
			}
		}

		for (i = 0; i < UD_LIGHTS; i++) {
			red[i + UD_LIGHT_OFFSET] = red[AXON_LIGHT_OFFSET + 9];
			green[i + UD_LIGHT_OFFSET] = green[AXON_LIGHT_OFFSET + 9];
			blue[i + UD_LIGHT_OFFSET] = blue[AXON_LIGHT_OFFSET + 9];
		}

		for (i = 0; i < LD_LIGHTS; i++) {
			red[i + LD_LIGHT_OFFSET] = red[AXON_LIGHT_OFFSET];
			green[i + LD_LIGHT_OFFSET] = green[AXON_LIGHT_OFFSET];
			blue[i + LD_LIGHT_OFFSET] = blue[AXON_LIGHT_OFFSET];
		}

		usleep(1000000/100);
	}
}
