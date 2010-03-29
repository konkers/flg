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

#include <Brain.hh>

int brain_poof(lua_State *l)
{
	Brain *b = (Brain *)lua_tointeger(l, lua_upvalueindex(1));
	b->poof(lua_tointeger(l, 1),
		lua_tointeger(l, 2),
		lua_tointeger(l, 3));
	return 0;
}

int brain_led(lua_State *l)
{
	Brain *b = (Brain *)lua_tointeger(l, lua_upvalueindex(1));
	b->led(lua_tointeger(l, 1),
	       lua_tointeger(l, 2),
	       lua_tointeger(l, 3),
	       lua_tointeger(l, 4));
	return 0;
}

int brain_sw(lua_State *l)
{
	Brain *b = (Brain *)lua_tointeger(l, lua_upvalueindex(1));
	lua_pushboolean(l, b->sw(lua_tointeger(l, 1),
				 lua_tointeger(l, 2)));
	return 1;
}


void Brain::poof(uint8_t addr, uint8_t relay, int duration)
{
	printf("poof(addr=%02x, relay=%d, duration=%d)\n", addr, relay, duration);
}

void Brain::led(uint8_t addr, uint8_t red, uint8_t green, uint8_t blue)
{
	printf("led(addr=%02x, red=%02x green=%02x, blue=%02x)\n",
	       addr, red, green, blue);
}

int Brain::sw(uint8_t addr, uint8_t idx)
{
	printf("sw(addr=%02x, idx=%02x)\n", addr, idx);
	return idx & 0x1;
}


void Brain::registerFunction(lua_CFunction func, const char *name)
{
	lua_pushlightuserdata(l, this);
	lua_pushcclosure(l, func, 1);
	lua_setglobal(l, name);
}

Brain::Brain()
{
	l = luaL_newstate();
	luaL_openlibs(l);

	registerFunction(brain_poof, "poof");
	registerFunction(brain_led, "led");
	registerFunction(brain_sw, "switch");

	lua_settop(l, 0);
}

Brain::~Brain()
{
	lua_close(l);
}


void Brain::runFile(const char *fileName)
{
	luaL_dofile(l, fileName);
}

