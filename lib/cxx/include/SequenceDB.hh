/*
 * Copyright 2010 Erik Gilling
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

#ifndef __SequenceDB_hh__
#define __SequenceDB_hh__

#include <string>
#include <map>
using namespace std;

#include <Sequence.hh>

class SequenceDB
{
private:
	class SeqInfo
	{
	public:
		int refCount;
		Sequence *seq;
	};

	map<string, Sequence *> sequences;

	const vector<string> *pixelMap;

	void loadDir(string dirName);

public:
	SequenceDB();

	void load(const vector<string> *pixelMap, const char *dirName);
	Sequence *get(const char *name);
	void put(Sequence *seq);
};

#endif /* __SequenceDB_hh__ */

