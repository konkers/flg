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

#ifndef __util_hh__
#define __util_hh__

#include <stdint.h>
#include <stdlib.h>

static inline bool get_uint8(const char *s, uint8_t *val)
{
	char *endp;
	long v;

	v = strtol(s, &endp, 0);
	if (endp == NULL || endp == s)
		return false;
	if (v < 0 || v > 0xff)
		return false;

	*val = v & 0xff;
	return true;
}

#endif /* __util_hh__ */


