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

#ifndef __Brain_hh__
#define __Brain_hh__

#include <stdint.h>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

class Brain {
private:
	lua_State *l;
	friend int brain_poof(lua_State *l);
	friend int brain_led(lua_State *l);
	friend int brain_sw(lua_State *l);

	void poof(uint8_t addr, uint8_t relay, int duration);
	void led(uint8_t addr, uint8_t red, uint8_t green, uint8_t blue);
	int sw(uint8_t addr, uint8_t idx);

	void registerFunction(lua_CFunction func, const char *name);

public:
	Brain();
	~Brain();

	void runFile(const char *fileName);
};


#endif /* __Brain_hh__ */

