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

#include <MapFileParser.hh>
#include <FtdiLink.hh>

#include "Soma.hh"
#include "Thread.hh"

Soma *soma;

int main(int argc, char *argv[])
{
	FtdiLink *flameLink;
//	FtdiLink *ledLink;

	flameLink = new FtdiLink(0x0403, 0x6010, INTERFACE_A);
//	ledLink = new FtdiLink(0x0403, 0x6010, INTERFACE_B);


	soma = new Soma();


	if (!soma->loadConfig("soma.xml"))
		return 1;

	soma->attachLink("flame", flameLink);
//	soma->attachLink("led", ledLink);

//	state->startWebServer(8080);

	soma->run();
}

