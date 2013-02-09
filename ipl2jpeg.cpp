// Copyright 2013 Benjamin Reh <ich@benjaminreh.de>
 // 
 // This program is free software; you can redistribute it and/or modify
 // it under the terms of the GNU General Public License as published by
 // the Free Software Foundation; either version 2 of the License, or
 // (at your option) any later version.
 // 
 // This program is distributed in the hope that it will be useful,
 // but WITHOUT ANY WARRANTY; without even the implied warranty of
 // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 // GNU General Public License for more details.
 // 
 // You should have received a copy of the GNU General Public License
 // along with this program; if not, write to the Free Software
 // Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 // MA 02110-1301, USA.
#include "ipl2jpeg.h"
bool ipl2jpeg(IplImage *frame, unsigned char **outbuffer, long unsigned int *outlen) {
unsigned char *outdata = (uchar *) frame->imageData;
struct jpeg_compress_struct cinfo = {0};
struct jpeg_error_mgr jerr;
JSAMPROW row_ptr[1];
int row_stride;

*outbuffer = NULL;
*outlen = 0;

cinfo.err = jpeg_std_error(&jerr);
jpeg_create_compress(&cinfo);
jpeg_mem_dest(&cinfo, outbuffer, outlen);

cinfo.image_width = frame->width;
cinfo.image_height = frame->height;
cinfo.input_components = frame->nChannels;
cinfo.in_color_space = JCS_RGB;

jpeg_set_defaults(&cinfo);
jpeg_start_compress(&cinfo, TRUE);
row_stride = frame->width * frame->nChannels;

while (cinfo.next_scanline < cinfo.image_height) {
    row_ptr[0] = &outdata[cinfo.next_scanline * row_stride];
    jpeg_write_scanlines(&cinfo, row_ptr, 1);
}

jpeg_finish_compress(&cinfo);
jpeg_destroy_compress(&cinfo);

return true;

}
