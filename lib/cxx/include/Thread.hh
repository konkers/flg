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

#ifndef __Thread_hh__
#define __Thread_hh__

#include <pthread.h>

class Thread
{
public:
	class Mutex
	{
	private:
		pthread_mutex_t mutex;

	public:
		Mutex() {
			pthread_mutex_init(&mutex, NULL);
		}

		~Mutex() {
			pthread_mutex_destroy(&mutex);
		}

		void lock(void) {
			pthread_mutex_lock(&mutex);
		}

		bool tryLock(void) {
			int err;

			err = pthread_mutex_trylock(&mutex);

			return err < 0 ? false : true;
		}

		void unlock(void) {
			pthread_mutex_unlock(&mutex);
		}
	};

	class Condition
	{
	private:
		pthread_mutex_t mutex;
		pthread_cond_t cond;

	public:
		Condition() {
			pthread_mutex_init(&mutex, NULL);
			pthread_cond_init(&cond, NULL);
		}

		~Condition() {
			pthread_mutex_destroy(&mutex);
			pthread_cond_destroy(&cond);
		}

		void lock(void) {
			pthread_mutex_lock(&mutex);
		}

		void unlock(void) {
			pthread_mutex_unlock(&mutex);
		}

		void wait(void) {
			pthread_cond_wait(&cond, &mutex);
		}

		void signal(void) {
			pthread_cond_signal(&cond);
		}
	};

private:
	bool running;
	pthread_t pthread;
	int rc;

	friend void *ThreadStart(void * data);

protected:
	virtual int run(void) = 0;

public:
	Thread();
	virtual ~Thread();

	bool start(void);
	int stop(void);
};


#endif /* __Thread_hh__ */
