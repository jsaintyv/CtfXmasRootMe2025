#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <tiffio.h>
#include <png.h>

extern void write_png(const char *filename, uint32 *buffer, uint32 width, uint32 height);

extern int generateImageClip(uint32 rect_width, uint32 rect_height, uint32 *rect_buf, uint32 y1, uint32 y2, uint32 tile_height, uint32 x1, uint32 x2, uint32 tile_width, TIFF *tif, uint8 *tile_buf, uint32 width, uint32 height);

