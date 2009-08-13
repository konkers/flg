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

#define AXON_RELAYS		24
#define AXON_RELAY_OFFSET	0

#define SPARKLE_RELAYS		5
#define SPARKLE_RELAY_OFFSET	(AXON_RELAY_OFFSET + AXON_RELAYS)

#define N_RELAYS		(AXON_RELAYS + SPARKLE_RELAYS)

#define N_BUTTONS		11

#define PHASE_DELAY		1000

static uint8_t red[N_LIGHTS];
static uint8_t green[N_LIGHTS];
static uint8_t blue[N_LIGHTS];
//static uint8_t state[N_LIGHTS];
static int phase[N_LIGHTS];

static uint8_t relays[N_RELAYS];

static void state_page(struct mg_connection *conn,
	    const struct mg_request_info *ri, void *data)
{
	int i;

	mg_printf(conn, "HTTP/1.1 200 OK\r\n"
		  "content-Type: text/plain\r\n\r\n");

	for (i = 0; i < N_LIGHTS; i++) {
		mg_printf(conn, "%02x: %06x\r\n", i,
			  (red[i] << 16) + (green[i] << 8) + blue[i]);
	}

	for (i = 0; i < N_RELAYS; i++) {
		mg_printf(conn, "%02x: %x\r\n", i + 0x80, relays[i]);

	}
}

static void button_page(struct mg_connection *conn,
	    const struct mg_request_info *ri, void *data)
{
	char *num;
	char *dir;

	char *p;
	int button;
	int s;

	/* XXX: probably buffer overflow action here */
	dir = strrchr(ri->uri, '/');
	*dir = '\0';
	dir++;

	num = strrchr(ri->uri, '/');
	num++;

	button = strtoul(num, &p, 0);
	if (p == NULL || p == num ) {
		printf("bad integer %s\n", num);
		return;
	}

	if (button < 0 || button >= N_BUTTONS) {
		printf("button %d out of range\n", button);
		return;
	}

	if (!strcmp(dir, "down")) {
		s = 1;
	} else if (!strcmp(dir, "up")) {
		s = 0;
	} else {
		printf("unkown button action %s\n", dir);
		return;
	}

	if (button > 0 && button < 9) {
		button -= 1;
		relays[button * 3] = s;
		relays[button * 3 + 1] = s;
		relays[button * 3 + 2] = s;
	} else {
		printf("button: %d %s\n", button, dir);
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
	mg_set_uri_callback(ctx, "/soma/button/*", &button_page, NULL);

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
