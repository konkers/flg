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

#include <MapFileParser.hh>

#include "Soma.hh"
#include "FlameProtoThread.hh"
#include "LedProtoThread.hh"

Soma::Soma()
{
	int s;
	int i;

	ledIdx = 0;
	flameIdx = 0;

	for (s = 0; s < 2; s++) {
		for(i = 0; i < nLights; i++)
			state[s].lights[i] = 0x0;

		for(i = 0; i < nRelays; i++)
			state[s].relays[i] = false;

		for(i = 0; i < nButtons; i++)
			state[s].buttons[i] = false;

		for(i = 0; i < nKnobs; i++)
			state[s].knobs[i] = 0x0;
	}

	for(i = 0; i < nRelays; i++)
		relayTimeout[i] = 0;

	ledProtoThread = NULL;
	flameProtoThread = NULL;

	ledLocks[0].lock();
	ledLocks[1].lock();

	flameLocks[0].lock();
	flameLocks[1].lock();

	initLua();
}

Soma::~Soma()
{
	if (ledProtoThread)
		delete ledProtoThread;

	if (flameProtoThread)
		delete flameProtoThread;
}

void Soma::attachLedLink(Link *l)
{
	ledProtoThread = new LedProtoThread(this, l);
	ledProtoThread->start();
}

void Soma::attachFlameLink(Link *l)
{
	flameProtoThread = new FlameProtoThread(this, l);
	flameProtoThread->start();
}

bool Soma::loadAttrMap(char *fileName, EffectAttr *attrs, int nAttrs)
{
	MapFileParser p;
	char *key;
	int val;
	int i = 0;

	if (!p.open(fileName))
		return false;

	while (p.read(&key, &val)) {
		if (i >= nAttrs) {
			fprintf(stderr, "too many lines in %s\n", fileName);
			return false;
		}
		attrs[i].name = key;
		attrs[i].val = val;
		i++;
	}

	return true;
}

bool Soma::loadLightMap(char *fileName)
{
	if (!loadAttrMap(fileName, lightAttrs, nLights))
		return false;

	luaCreateAttrTable("leds", lightAttrs, nLights);
	return true;
}

bool Soma::loadRelayMap(char *fileName)
{
	return loadAttrMap(fileName, relayAttrs, nRelays);
}

bool Soma::loadDpotMap(char *fileName)
{
	return loadAttrMap(fileName, dpotAttrs, nDpots);
}

bool Soma::loadButtonMap(char *fileName)
{
	return loadAttrMap(fileName, buttonAttrs, nButtons);
}

bool Soma::loadKnobMap(char *fileName)
{
	return loadAttrMap(fileName, knobAttrs, nKnobs);
}


void Soma::run(void)
{
	struct timeval tv;
	struct timeval last_tv;
	struct timeval tmp_tv;
	struct timeval frametime;
	int i = 0;

	frametime.tv_sec = 0;
	frametime.tv_usec = 10000;

	gettimeofday(&last_tv, NULL);
	while(1) {
		sync();
		processFrame(i++);

		gettimeofday(&tv, NULL);
		timersub(&tv, &last_tv, &tmp_tv);
		if (timercmp(&tmp_tv, &frametime, <)) {
			timersub(&frametime, &tmp_tv, &tv);
			usleep(tv.tv_usec);
		} else {
			fprintf(stderr, "frame overrun: ");
			timersub(&flameSyncTime, &last_tv, &tmp_tv);
			fprintf(stderr, "flameSyncDelay: %f ", 1.0 * tmp_tv.tv_sec +
			       (1.0 * tmp_tv.tv_usec)/(1000000.0));
			timersub(&ledSyncTime, &last_tv, &tmp_tv);
			fprintf(stderr, "ledSyncDelay: %f ", 1.0 * tmp_tv.tv_sec +
			       (1.0 * tmp_tv.tv_usec)/(1000000.0));
			fprintf(stderr, "\n");
		}
		gettimeofday(&last_tv, NULL);
	}
}

void Soma::processFrame(int frame)
{
	int i;
	int ret;

	lua_getfield(l, -1, "process");
	lua_pushvalue(l, 1);

	ret = lua_pcall(l, 1, 0, 0);
	if (ret != 0) {
		printf(" err: %s\n", lua_tostring(l, -1));
	}

	for (i = 0; i < nButtons; i++) {
		if (button(i)) {
			enableRelay(i * 3, 10);
			enableRelay(i * 3 + 1, 10);
			enableRelay(i * 3 + 2, 10);
		}
	}

	for (i = 0; i < nRelays; i++) {
		if (relayTimeout[i] > 0) {
			setRelay(i, true);
			relayTimeout[i]--;
		} else {
			setRelay(i, false);
		}
	}
}

void Soma::sync(void)
{
	flameLocks[!flameIdx].lock();
	ledLocks[!ledIdx].lock();

	// we now hold all locks.
	// It is safe to update flameIdx and ledIdx
	flameIdx = !flameIdx;
	ledIdx = !ledIdx;

	// now let the link threads run
	flameLocks[!flameIdx].unlock();
	ledLocks[!ledIdx].unlock();
}

void Soma::flameSync(void)
{
	int newIdx = !flameIdx;

	gettimeofday(&flameSyncTime, NULL);

	// Soma is holding flameIdx
	// flameLink is holding newIdx
	flameLocks[newIdx].unlock();

	// Soma will update flameIdx before releasing this lock
	flameLocks[!newIdx].lock();
}

void Soma::ledSync(void)
{
	int newIdx = !ledIdx;

	gettimeofday(&ledSyncTime, NULL);

	// Soma is holding ledIdx
	// ledLink is holding newIdx

	ledLocks[newIdx].unlock();

	// Soma will update ledIdx before releasing this lock
	ledLocks[!newIdx].lock();
}

bool Soma::initLua(void)
{
	int ret;

	l = luaL_newstate();
	luaL_openlibs(l);

	lua_settop(l, 0);

	ret = luaL_dofile(l, "globals.lua");
	if (ret != 0) {
		printf("can't load globals file: %s\n", lua_tostring(l, -1));
		return false;
	}
	printf("%d\n", lua_gettop(l));
	ret = luaL_dofile(l, "test.lua");
	if (ret != 0) {
		printf("can't load file: %s\n", lua_tostring(l, -1));
		return false;
	}
	printf("%d\n", lua_gettop(l));

	luaRegisterFunction(lua_set_led, "set_led");

	return true;
}

void Soma::luaRegisterFunction(lua_CFunction func, const char *name)
{
	lua_pushlightuserdata(l, this);
	lua_pushcclosure(l, func, 1);
	lua_setglobal(l, name);
}

void Soma::luaCreateAttrTable(const char *name, const EffectAttr *attrs, int n)
{
	int i;

	lua_createtable(l, n, 0);

	for (i = 0; i < n; i++) {
		printf("set %s = %d\n", attrs[i].name.c_str(), i);
		lua_pushstring(l, attrs[i].name.c_str());
		lua_pushinteger(l, i);
		lua_settable(l, -3);
	}

	lua_setglobal(l, name);
}



int lua_get_led(lua_State *l)
{
	return 0;
}

int lua_set_led(lua_State *l)
{
	Soma *soma = (Soma *)lua_touserdata(l, lua_upvalueindex(1));

	uint8_t red;
	uint8_t green;
	uint8_t blue;


	if (lua_gettop(l) != 4) {
		lua_pushfstring(l, "set_led wrong number of arguments.  Expected 4. got %d",
				lua_gettop(l));
		lua_error(l);
		return 0;
	}

	if (!lua_isnumber(l, 1)) {
		lua_pushfstring(l, "set_led arg 1 not an integer");
		lua_error(l);
	}

	red = lua_tointeger(l, 2);
	green = lua_tointeger(l, 3);
	blue = lua_tointeger(l, 4);

	soma->setLight(lua_tointeger(l,1), red, green, blue);
	return 0;
}



int lua_get_relay(lua_State *l)
{
	return 0;
}

int lua_set_relay(lua_State *l)
{
	return 0;
}


int lua_get_dpot(lua_State *l)
{
	return 0;
}

int lua_set_dpot(lua_State *l)
{
	return 0;
}


int lua_get_knob(lua_State *l)
{
	return 0;
}

int lua_get_button(lua_State *l)
{
	return 0;
}

