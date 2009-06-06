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
	b->poof(lua_tointeger(l, 1),lua_tointeger(l, 2),lua_tointeger(l, 3));
	return 0;
}

void Brain::poof(uint8_t addr, uint8_t relay, int duration)
{
	printf("poof(%02x, %d, %d)\n", addr, relay, duration);
}


Brain::Brain()
{
	l = luaL_newstate();
	luaL_openlibs(l);
	lua_pushinteger(l, (unsigned int)this);
	lua_pushcclosure(l, brain_poof, 1);
	lua_setglobal(l, "poof");
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

