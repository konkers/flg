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

#include <MapFileParser.hh>

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

MapFileParser::MapFileParser()
{
	fd = -1;
}

MapFileParser::~MapFileParser()
{
	if (fd >= 0) {
		::close(fd);
	}
}


bool MapFileParser::open(const char *fileName)
{
	fd = ::open(fileName, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "MapFileParser: can't open file '%s': %s\n",
			fileName, strerror(errno));
		return false;
	}

	return true;
}

bool MapFileParser::read(char **key, int *val)
{
	int i = 0;
	char *valStr;
	int err;

	while (i < readBuffLen) {
		err = ::read(fd, readBuff + i, 1);
		if (err == 0)
			break;

		if (err < 0) {
			//XXX-ewg: does not handle EAGAIN
			fprintf(stderr, "MapFileParser: err reading file: %s\n",
				strerror(errno));
			goto err;
		}

		if (readBuff[i] == '\n')
			break;
		i++;
	}

	// end of file
	if (i == 0)
		goto err;

	readBuff[i] = '\0';

	valStr = strchr(readBuff, ':');
	if (valStr == NULL) {
		fprintf(stderr, "MapFileParser: \"%s\" contains no ':'\n",
			readBuff);
		goto err;
	}

	*valStr = '\0';
	valStr++;

	*key = readBuff;
	*val = strtol(valStr, NULL, 0);

	return true;

err:
	close(fd);
	fd = -1;
	return false;
}

