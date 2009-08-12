#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "mongoose.h"


static uint8_t red[10];
static uint8_t green[10];
static uint8_t blue[10];
static uint8_t state[10];


static void
state_page(struct mg_connection *conn,
	    const struct mg_request_info *ri, void *data)
{
	int i;

	mg_printf(conn, "HTTP/1.1 200 OK\r\n"
		  "content-Type: text/plain\r\n\r\n");

	for (i = 0; i < 10; i++) {
		switch (state[i]) {
		case 0:
			green[i]++;
			if (green[i] == 0xff)
				state[i]++;
			break;

		case 1:
			red[i]--;
			if (red[i] == 0x00)
				state[i]++;
			break;

		case 2:
			blue[i]++;
			if (blue[i] == 0xff)
				state[i]++;
			break;

		case 3:
			green[i]--;
			if (green[i] == 0x00)
				state[i]++;
			break;

		case 4:
			red[i]++;
			if (red[i] == 0xff)
				state[i]++;
			break;

		case 5:
			blue[i]--;
			if (blue[i] == 0x00)
				state[i] = 0;
			break;
		}
		mg_printf(conn, "%02x: %06x\r\n", i,
			  (red[i] << 16) + (green[i] << 8) + blue[i]);

	}
}


int
main(int argc, char *argv[])
{
	struct mg_context *ctx;
	int i;

	for (i = 0; i < 10; i++) {
		state[i] = i%6;
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

	for (;;)
		sleep(1);
}
