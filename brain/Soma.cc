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

#include <string.h>

#include <pcre++.h>

using namespace pcrepp;

#include "Soma.hh"

#include "mongoose.h"

void channel_page(struct mg_connection *conn,
		const struct mg_request_info *ri, void *data)
{
	Soma *s = (Soma *)data;
	s->channelPage(conn, ri);
}

void patterns_page(struct mg_connection *conn,
		   const struct mg_request_info *ri, void *data)
{
	Soma *s = (Soma *)data;
	s->patternsPage(conn, ri);
}

void rate_page(struct mg_connection *conn,
	       const struct mg_request_info *ri, void *data)
{
	Soma *s = (Soma *)data;
	s->ratePage(conn, ri);
}


Soma::Soma()
{
	axonLedNames.push_back("a1");
	axonLedNames.push_back("a2");
	axonLedNames.push_back("a3");
	axonLedNames.push_back("a4");
	axonLedNames.push_back("a5");
	axonLedNames.push_back("a6");
	axonLedNames.push_back("a7");
	axonLedNames.push_back("a8");
	axonLedNames.push_back("a9");
	axonLedNames.push_back("a10");

	lowerLedNames.push_back("l11");
	lowerLedNames.push_back("l12");
	lowerLedNames.push_back("l13");
	lowerLedNames.push_back("l14");
	lowerLedNames.push_back("l15");
	lowerLedNames.push_back("l16a");
	lowerLedNames.push_back("l16c");
	lowerLedNames.push_back("l17a");
	lowerLedNames.push_back("l17c");
	lowerLedNames.push_back("l18a");
	lowerLedNames.push_back("l18c");
	lowerLedNames.push_back("l19a");
	lowerLedNames.push_back("l19c");
	lowerLedNames.push_back("l20a");
	lowerLedNames.push_back("l20c");

	upperLedNames.push_back("u1");
	upperLedNames.push_back("u2");
	upperLedNames.push_back("u3");
	upperLedNames.push_back("u4");
	upperLedNames.push_back("u5");
	upperLedNames.push_back("u6a");
	upperLedNames.push_back("u6c");
	upperLedNames.push_back("u7a");
	upperLedNames.push_back("u7c");
	upperLedNames.push_back("u8a");
	upperLedNames.push_back("u8c");
	upperLedNames.push_back("u9a");
	upperLedNames.push_back("u9c");
	upperLedNames.push_back("u10a");
	upperLedNames.push_back("u10c");
	upperLedNames.push_back("u11a");
	upperLedNames.push_back("u11c");
	upperLedNames.push_back("u12a");
	upperLedNames.push_back("u12c");
	upperLedNames.push_back("u13a");
	upperLedNames.push_back("u13c");
	upperLedNames.push_back("u14a");
	upperLedNames.push_back("u14c");
	upperLedNames.push_back("u15a");
	upperLedNames.push_back("u15c");
	upperLedNames.push_back("u16a");
	upperLedNames.push_back("u16c");
	upperLedNames.push_back("u17a");
	upperLedNames.push_back("u17c");
	upperLedNames.push_back("u18a");
	upperLedNames.push_back("u18c");
	upperLedNames.push_back("u19a");
	upperLedNames.push_back("u19c");
	upperLedNames.push_back("u20a");
	upperLedNames.push_back("u20c");

	allLedNames.insert(allLedNames.end(),
			   lowerLedNames.begin(), lowerLedNames.end());
	allLedNames.insert(allLedNames.end(),
			   axonLedNames.begin(), axonLedNames.end());
	allLedNames.insert(allLedNames.end(),
			   upperLedNames.begin(), upperLedNames.end());

	digitalNames.push_back("a1a");
	digitalNames.push_back("a1b");
	digitalNames.push_back("a1c");
	digitalNames.push_back("a2a");
	digitalNames.push_back("a2b");
	digitalNames.push_back("a2c");
	digitalNames.push_back("a3a");
	digitalNames.push_back("a3b");
	digitalNames.push_back("a3c");
	digitalNames.push_back("a4a");
	digitalNames.push_back("a4b");
	digitalNames.push_back("a4c");
	digitalNames.push_back("a5a");
	digitalNames.push_back("a5b");
	digitalNames.push_back("a5c");
	digitalNames.push_back("a6a");
	digitalNames.push_back("a6b");
	digitalNames.push_back("a6c");
	digitalNames.push_back("a7a");
	digitalNames.push_back("a7b");
	digitalNames.push_back("a7c");
	digitalNames.push_back("a8a");
	digitalNames.push_back("a8b");
	digitalNames.push_back("a8c");
	digitalNames.push_back("s1");
	digitalNames.push_back("s2");
	digitalNames.push_back("s3");
	digitalNames.push_back("s4");
	digitalNames.push_back("s5");

	allNames.insert(allNames.end(),
			   allLedNames.begin(), allLedNames.end());
	allNames.insert(allNames.end(),
			   digitalNames.begin(), digitalNames.end());
	seqDB.load(&allNames, "patterns");

	seq = new StepSequencer(8, &allNames);

	int i;
	for (i = 0; i < 8; i++)
		seq->setChannelSequence(i, seqDB.get("patterns/none.png"));

	printf("lower:%d axon:%d upper:%d poofer:%d\n",
	       (int) lowerLedNames.size(),
	       (int) axonLedNames.size(),
	       (int) upperLedNames.size(),
	       (int) digitalNames.size());
}

Soma::~Soma()
{

}

bool Soma::loadConfig(const char *fileName, const char *eventsfileName)
{
	return state.loadConfig(fileName);
}

bool Soma::attachLink(const char *busName, Link *link)
{
	return state.attachLink(busName, link);
}

#define min(x, y)	((x) < (y) ? (x) : (y))

uint8_t Soma::handleMotor(uint16_t input, uint8_t value,
			  uint8_t max, uint8_t slewRate)
{
	uint8_t out;

	out = input * max / 0x400;

	if (out < value)
		value -= min(value - out, slewRate);
	else if (out > value)
		value += min(out - value, slewRate);

	return out;
}


void Soma::startWebServer(int port)
{
	char buff[16];
	snprintf(buff, 16, "%d", port);
	mg_ctx = mg_start();
	mg_set_option(mg_ctx, "ports", buff);
	mg_set_uri_callback(mg_ctx, "/channel*", &channel_page, this);
	mg_set_uri_callback(mg_ctx, "/patterns", &patterns_page, this);
	mg_set_uri_callback(mg_ctx, "/rate*", &rate_page, this);
}

void Soma::channelPage(struct mg_connection *conn,
		       const struct mg_request_info *ri)
{
	Pcre getStateReg("^/channel/(\\d+)/state$");
	Pcre setStateReg("^/channel/(\\d+)/state/(off|single|loop)$");
	Pcre getPatternReg("^/channel/(\\d+)/pattern$");
	Pcre setPatternReg("^/channel/(\\d+)/pattern/(.+)$");
	int state;
	unsigned int channel;
	Sequence *s;

	mg_printf(conn, "HTTP/1.1 200 OK\r\n"
		  "content-Type: text/plain\r\n\r\n");

	if (!strcmp(ri->uri, "/channel")) {
		seqLock.lock();
		mg_printf(conn, "{\"result\": true, \"channels\": %d}\r\n", seq->getNumChannels());
		seqLock.unlock();
	} else if (getStateReg.search(ri->uri)) {
		const char *val;

		channel = atoi(getStateReg.get_match(0).c_str());
		seqLock.lock();
		state = seq->getChannelState(channel);
		seqLock.unlock();

		if (state == -1) {
			mg_printf(conn, "{\"result\": flase, \"error\": \"channel %d out of range\"}\r\n", channel);
		} else {
			if (state == StepSequencer::STATE_SINGLE)
				val = "single";
			else if (state == StepSequencer::STATE_LOOP)
				val = "loop";
			else
				val = "off";

			mg_printf(conn, "{\"result\": true, \"state\": \"%s\"}\r\n", val);
		}
	} else if (setStateReg.search(ri->uri)) {
		channel = atoi(setStateReg.get_match(0).c_str());

		if(!setStateReg.get_match(1).compare("loop"))
			state = StepSequencer::STATE_LOOP;
		else if(!setStateReg.get_match(1).compare("single"))
			state = StepSequencer::STATE_SINGLE;
		else
			state = StepSequencer::STATE_OFF;

		seqLock.lock();
		seq->setChannelState(channel, state);
		seqLock.unlock();
		mg_printf(conn, "{\"result\": true, \"msg\": \"set state %d %d\"}\r\n", channel, state);
	} else if (getPatternReg.search(ri->uri)) {
		channel = atoi(getPatternReg.get_match(0).c_str());

		seqLock.lock();
		s = seq->getChannelSequence(channel);
		if (s)
			mg_printf(conn, "{\"result\": true, \"pattern\": \"%s\"}\r\n", s->getName().c_str());
		else
			mg_printf(conn, "{\"result\": flase, \"error\": \"channel %d out of range\"}\r\n", channel);
		seqLock.unlock();
	} else if (setPatternReg.search(ri->uri)) {
		channel = atoi(setPatternReg.get_match(0).c_str());

		seqLock.lock();
		s = seqDB.get(setPatternReg.get_match(1).c_str());
		seq->setChannelSequence(channel, s);
		seqLock.unlock();
		mg_printf(conn, "{\"result\": true, \"msg\": \"set pattern %d %s\"}\r\n", channel, s->getName().c_str());
	} else {
		mg_printf(conn, "channel %s\r\n", ri->uri);
	}

}

void Soma::patternsPage(struct mg_connection *conn,
			const struct mg_request_info *ri)
{
	map<string, Sequence *>::iterator i;

	mg_printf(conn, "HTTP/1.1 200 OK\r\n"
		  "content-Type: text/plain\r\n\r\n");


	mg_printf(conn, "{\"result\": true, \"patterns\": \"");
	seqLock.lock();

	for (i = seqDB.begin(); i != seqDB.end(); i++) {
		mg_printf(conn, "%s\\n", i->first.c_str());
	}
	seqLock.unlock();
	mg_printf(conn, "\"}\r\n");
}

void Soma::ratePage(struct mg_connection *conn,
		       const struct mg_request_info *ri)
{
	mg_printf(conn, "HTTP/1.1 200 OK\r\n"
		  "content-Type: text/plain\r\n\r\n");

	mg_printf(conn, "{\"result\":true, \"rate\": %d}\r\n", 12000);
}

void Soma::run(void)
{
	struct timeval tv;
	struct timeval last_tv;
	struct timeval tmp_tv;
	struct timeval frametime;
	uint8_t motor1 = 0;
	uint8_t motor2 = 0;

	vector<string>::iterator i;

	frametime.tv_sec = 0;
	frametime.tv_usec = 33000;

	state.run();

	startWebServer(1080);

	printf("run\n");

	// XXX: gettimeofday is not garunteed to be regular as the clock can be
	//      set at any time (by user, ntp, etc.)
	gettimeofday(&last_tv, NULL);
	while (1) {
		state.sync();

		seqLock.lock();
		seq->step(&state);
		seqLock.unlock();

		state.setDigitalOut("unpower1", true);
		state.setDigitalOut("unpower2", true);

		motor1 = handleMotor(state.getAnalogIn("u_knob_a"),
				     motor1, 0x80, 0x1);
		motor2 = handleMotor(state.getAnalogIn("u_knob_b"),
				     motor2, 0x80, 0x1);

		state.setAnalogOut("uns1", motor1);
		state.setAnalogOut("uns2", motor1);
		state.setAnalogOut("lns1", motor2);
		state.setAnalogOut("lns2", motor2);

		gettimeofday(&tv, NULL);
		timersub(&tv, &last_tv, &tmp_tv);
		if (timercmp(&tmp_tv, &frametime, <)) {
			timersub(&frametime, &tmp_tv, &tv);
			usleep(tv.tv_usec);
		} else {
			fprintf(stderr, "frame overrun: %d %d",
				(int)tmp_tv.tv_sec, (int)tmp_tv.tv_usec);
			fprintf(stderr, "\n");
		}
		gettimeofday(&last_tv, NULL);

	}
}


