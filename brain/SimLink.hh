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

#ifndef __SimLink_hh__
#define __SimLink_hh__

#include <stdint.h>

#include <Link.hh>

#include "SimState.hh"

class SimLink : public Link {
private:
	SimState *s;

public:
	SimLink(SimState *state);
	virtual ~SimLink();

	virtual bool send(const void *data, int len);
	virtual int wait(int timeout);
	virtual bool recv(void *data, int *len);
};

#endif /* __SimLink_hh__ */
