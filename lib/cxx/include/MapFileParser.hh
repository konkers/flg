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

#ifndef __MapFileParser_hh__
#define __MapFileParser_hh__


class MapFileParser {
private:
	int fd;

	static const int readBuffLen = 512;
	char readBuff[readBuffLen];

public:
	MapFileParser();
	~MapFileParser();

	bool open(const char *fileName);

	bool read(char **key, int *val);
};

#endif /* __MapFileParser_hh__ */

