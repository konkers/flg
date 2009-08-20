#include <sys/time.h>

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

	ledProtoThread = NULL;
	flameProtoThread = NULL;

	ledLocks[0].lock();
	ledLocks[1].lock();

	flameLocks[0].lock();
	flameLocks[1].lock();
}

Soma::~Soma()
{
	if (ledProto)
		delete ledProto;

	if (flameProto)
		delete flameProto;
}


void Soma::attachLedLink(Link *l)
{
	ledProto = new LedProtoThread(this, l);
	ledProto->start();
}

void Soma::attachFlameLink(Link *l)
{
	flameProto = new FlameProtoThread(this, l);
	flameProto->start();
}

void Soma::run(void)
{
	struct timeval tv;
	struct timeval last_tv;
	struct timeval tmp_tv;
	struct timeval frametime;

	frametime.tv_sec = 0;
	frametime.tv_usec = 100000;

	gettimeofday(&last_tv, NULL);
	while(1) {
		sync();

		processFrame();

		gettimeofday(&tv, NULL);
		timersub(&tv, &last_tv, &tmp_tv);
		if (timercmp(&tmp_tv, &frametime, <)) {
			timersub(&frametime, &tmp_tv, &tv);
			usleep(tv.u_sec);
			gettimeofday(&last_tv, NULL);
		} else {
			fprintf(stderr, "frame overrun!\n");
		}
	}
}

void Soma::sync(void)
{
	flameLocks[!flameIdx].lock();
	flameLocks[!ledIdx].lock();

	// we now hold all locks.
	// It is safe to update flameIdx and ledIdx
	flameIdx = !flameIdx;
	ledIdx = !ledIdx;

	// now let the link threads run
	flameLocks[!flameIdx].unlock();
	ledLocks[!ledIdx].unlock();
}

void Soma::sync(void)
{
}


void Soma::flameSync(void)
{
	int newIdx = !flameIdx;

	// Soma is holding flameIdx
	// flameLink is holding newIdx

	flameLocks[newIdx].unlock();

	// Soma will update flameIdx before releasing this lock
	flameLocks[!newIdx].lock();
}

void Soma::ledSync(void)
{
	int newIdx = !ledIdx;

	// Soma is holding ledIdx
	// ledLink is holding newIdx

	ledLocks[newIdx].unlock();

	// Soma will update ledIdx before releasing this lock
	ledLocks[!newIdx].lock();
}

