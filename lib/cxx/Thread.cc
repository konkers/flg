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
#include <errno.h>
#include <string.h>

#include <Thread.hh>

void *ThreadStart(void * data)
{
	Thread *t = (Thread *) data;

	t->running = true;
	t->rc = t->run();
	pthread_exit(NULL);
}

Thread::Thread()
{
	running = false;
}

Thread::~Thread()
{
	stop();
}


bool Thread::start(void)
{
	int ret;

	if (!running) {
		ret = pthread_create(&pthread, NULL, ThreadStart, this);
		if (ret < 0) {
			fprintf(stderr, "pthread_create failed: %s\n",
				strerror(errno));
			return false;
		}
	}
	return true;
}

int Thread::stop(void)
{
	void *val;
	int ret;

	if (running) {
		ret = pthread_join(pthread, &val);
		if (ret < 0)
			fprintf(stderr, "pthread_join failed: %s\n",
				strerror(errno));
		running = false;
	}
	return rc;
}

