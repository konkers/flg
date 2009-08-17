
#ifndef __brain_sim_state_hh__
#define __brain_sim_state_hh__

#include <proto.h>

#include <vector>
#include <map>
#include <string>

using namespace std;

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
	uint8_t outputBuff[outputBuffSize];

	vector<SimProto *> protos;
	map<string, int> ledMapping;

	struct proto_handlers handlers;

	void populateMapping(void);

public:
	SimState(void);

	bool loadLightMapping(const char * fileName);

	void addLedRgb(string name, uint8_t addr);
	void addRelay3(string name, uint8_t addr);

	void send(uint8_t c);
	void ping(void);
	bool hasData(void);
};



#endif /* __brain_sim_state_hh__ */
