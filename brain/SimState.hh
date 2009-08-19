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

#ifndef __brain_sim_state_hh__
#define __brain_sim_state_hh__

#include <vector>
#include <map>
#include <string>
using namespace std;


#include <proto.h>

#include <RingBuff.hh>

class SimState
{
private:
	enum SimProtoType {
		RELAY3,
		LED_RGB,
	};

	class SimProto
	{
	public:
		SimProtoType type;
		string name;
		SimState *sim;
		struct proto p;
	};

	static const int outputBuffSize = 512;
	RingBuff *flameOutputBuff;
	RingBuff *ledOutputBuff;

	vector<SimProto *> flameProtos;
	vector<SimProto *> ledProtos;
	map<string, int> ledMapping;

	static const int nLights = 60;
	uint32_t lightState[nLights];

	struct proto_handlers flameHandlers;
	struct proto_handlers ledHandlers;

	struct mg_context *ctx;

	void populateMapping(void);

	friend void handle_relay(void *data, uint8_t idx, uint8_t state);
	friend void handle_long_data(void *data, uint32_t val);
	friend void handle_sync(void *data);
	friend void handle_flame_send(void *data, uint8_t *pkt_data, int len);
	friend void handle_led_send(void *data, uint8_t *pkt_data, int len);

	friend void state_page(struct mg_connection *conn,
			       const struct mg_request_info *ri, void *data);

	void handleLongData(SimProto *p, uint32_t val);
	void insertFlameByte(uint8_t c);
	void insertLedByte(uint8_t c);
	void statePage(struct mg_connection *conn,
		       const struct mg_request_info *ri);
public:
	SimState(void);

	bool loadLightMap(const char * fileName);

	void addLedRgb(string name, uint8_t addr);
	void addRelay3(string name, uint8_t addr);

	void startWebServer(int port);

	// XXX-ewg: not plumbed from anywhere... eeek  maybe start
	// a thread
	void ping(void);

	void flameSend(uint8_t c);
	uint8_t flameRecv(void);
	bool flameHasData(void);

	void ledSend(uint8_t c);
	uint8_t ledRecv(void);
	bool ledHasData(void);

};



#endif /* __brain_sim_state_hh__ */
