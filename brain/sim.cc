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

#include <stdio.h>

#include "Soma.hh"
#include "SimState.hh"
#include "SimLink.hh"
#include "Thread.h"

SimState *state;
SimLink *flameLink;
SimLink *ledLink;
Soma *soma;

class TestThread public Thread
{
protected:
	virtual int run(void)
	{
		while (1) {
			printf("test\n");
			sleep(1);
		}
	}
};

int main(int argc, char *argv)
{
	TestThread t;

	t.start();

	state = new SimState();
	flameLink = new SimLink(state, SimLink::FLAME);
	ledLink = new SimLink(state, SimLink::LED);

	soma = new Soma();
	soma.attachFlameLink(flameLink);
	soma.attachLedLink(ledLink);

	state.startWebServer(8080);

	soma.run();
}

