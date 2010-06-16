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

#include <png.h>

#include <PngSequence.hh>

PngSequence::PngSequence()
{
	data = NULL;
	h = 0;
	w = 0;
	lastRow = 0;
}

PngSequence::~PngSequence()
{
	if(data)
		delete data;
}


bool PngSequence::load(const char *fileName, unsigned frameLen)
{
	FILE *fp;
	png_structp png;
	png_infop info;
	png_bytepp pngdata;
	unsigned x, y;

	fp = fopen(fileName, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Png Sequence %s could not be opened for reading", fileName);
		return false;
	}

	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info = png_create_info_struct(png);

	png_init_io(png, fp);
	png_read_info(png, info);
	png_read_update_info(png, info);

        if (info->width != frameLen) {
		fprintf(stderr, "PngSequence %s is not the correct width (expected %u, got %lu).\n",
                        fileName, frameLen, info->width);
		goto err;
        }

	if (info->color_type != PNG_COLOR_TYPE_RGB &&
	    info->color_type != PNG_COLOR_TYPE_RGBA) {
		fprintf(stderr, "PngSequence %s has unsupported color type %d\n",
			fileName, info->color_type);
		goto err;
	}

	if (info->bit_depth != 8) {
		fprintf(stderr, "PngSequence %s has unsupported bith depth %d\n",
			fileName, info->bit_depth);
		goto err;
	}

	pngdata = (png_bytepp) malloc(sizeof(png_bytep) * info->height);
	for (y = 0; y < info->height; y++)
		pngdata[y] = (png_byte*) malloc(info->rowbytes);

	png_read_image(png, pngdata);

	h = info->height;
	w = info->width;
	data = new uint32_t[info->height * info->width];

	if (info->color_type == PNG_COLOR_TYPE_RGB) {
		for (y = 0; y < h; y++) {
			for (x = 0; x < w; x++) {
				png_byte *p = &pngdata[y][x * 3];
				if (p[0] > 5 || p[1] > 5 || p[2] > 5)
					data[y * w + x] =
						p[0] + (p[1] << 8) +
						(p[2] << 16) +
						(0xff << 24);
				else
					data[y * w + x] = 0x0;
			}
		}
	} else {
		for (y = 0; y < h; y++) {
			for (x = 0; x < w; x++) {
				uint8_t r, g, b, a;
				r = pngdata[y][x * 3];
				g = pngdata[y][x * 3 + 1];
				b = pngdata[y][x * 3 + 2];
				a = pngdata[y][x * 3 + 3];

				// pre-multiply the aplha channel
				r = (int)r * a / 255;
				g = (int)g * a / 255;
				b = (int)b * a / 255;

				data[y * w + x] =
					r + (g << 8) + (b << 16) + (a << 24);
			}
		}
	}

	for (y = 0; y < h; y++)
		free(pngdata[y]);
	free(pngdata);

#ifndef OSX
	png_read_destroy(png, info, NULL);
#endif
	fclose(fp);

	return true;

err:
#ifndef OSX
	png_read_destroy(png, info, NULL);
#endif
	fclose(fp);
	return false;
}


void PngSequence::start()
{
}

void PngSequence::stop(void)
{
}

bool PngSequence::handleFrame(uint32_t *frame, unsigned frameLen, float timeCode)
{
	unsigned row;
	unsigned i;
	bool looped;

	row = (int)(timeCode * 30.0) % h;
	looped = lastRow > row;

	for (i = 0; i < w; i++) {
		uint32_t d = data[row * w + i];
		uint8_t r, g, b, a;

		r = d & 0xff;
		g = (d >> 8) & 0xff;
		b = (d >> 16) & 0xff;
		a = (d >> 24) & 0xff;

		r += (int)(frame[i] & 0xff) * (0xff - a) / 0xff;
		g += (int)((frame[i] >> 8) & 0xff) * (0xff - a) / 0xff;
		b += (int)((frame[i] >> 16) & 0xff) * (0xff - a) / 0xff;

		if (a != 0x00 && a != 0xff) {
			printf("fuck %d\n", a);
		}
		frame[i] = r + (g << 8) + (b << 16);
	}

	lastRow = row;

	return looped;
}

