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

#include <stdio.h>
#include <string.h>

#include <dirent.h>

#include <PngSequence.hh>
#include <SequenceDB.hh>

void SequenceDB::loadDir(string dirName)
{
	DIR *dir;
	string fileName;
	struct dirent de, *de_p;

	dir = opendir(dirName.c_str());
	if (dir == NULL) {
		fprintf(stderr, "can't open directory %s\n", dirName.c_str());
		return;
	}

	readdir_r(dir, &de, &de_p);
	while (de_p != NULL) {
		if (strcmp(de.d_name, ".") && strcmp(de.d_name, "..")) {
			if (de.d_type == DT_REG) {
				PngSequence *s = new PngSequence;
				fileName = dirName + "/" + de.d_name;
				if (s->load(fileName.c_str(), pixelMap->size())) {
					s->get();
					sequences[fileName] = s;
				} else {
					delete s;
				}
			} else if(de.d_type == DT_DIR) {
				fileName = dirName + "/" + de.d_name;
				loadDir(fileName);
			} else {
				fprintf(stderr, "found %s with unknown file type%d\n",
					de.d_name, de.d_type);
			}
		}

		readdir_r(dir, &de, &de_p);
	}
}

SequenceDB::SequenceDB()
{
}

void SequenceDB::load(const vector<string> *pixelMap, const char *dirName)
{
	this->pixelMap = pixelMap;
	loadDir(dirName);

	map<string, Sequence *>::iterator i;

	for (i = sequences.begin(); i != sequences.end(); i++) {
		printf("%s\n", i->first.c_str());
	}
}

Sequence *SequenceDB::get(const char *name)
{
	map<string, Sequence *>::iterator i;

	i = sequences.find(name);

	if (i == sequences.end())
		return NULL;

	i->second->get();

	return i->second;
}

void SequenceDB::put(Sequence *seq)
{
	if (seq->put() == 0)
		delete seq;
}


