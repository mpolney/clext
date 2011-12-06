#include "types.h"
#include "mem.h"
#include "bmp.h"

struct bmp24_writer_t *newBMP24Writer(struct writer_t *w, LONG width, LONG height) {
	struct bmp24_writer_t *bw;
	bw = (struct bmp24_writer_t *)malloc2(sizeof(*bw));

	bw->w = w;
	bw->width = width;
	bw->height = height;

	return bw;
}

void deleteBMP24Writer(struct bmp24_writer_t *w) {
	free(w);
}

static LONG byteWidth(struct bmp24_writer_t *bw) {
	LONG r;
	LONG pixW;

	pixW = bw->width*3;
	r = pixW & 3;
	if (r == 0)
		return pixW;

	return (pixW & ~3) + 4;
}

void writeBMP24Header(struct bmp24_writer_t *bw) {
	setWriterByteOrder(bw->w, BYTEORDER_LE);

	writeShort(bw->w, 0x4d42); /* "BM" */
	writeLong(bw->w, 54 + byteWidth(bw)*bw->height);
	writeShort(bw->w, 0);
	writeShort(bw->w, 0);
	writeLong(bw->w, 54);

	writeLong(bw->w, 40); /* Header size */
	writeLong(bw->w, bw->width); /* Image width */
	writeLong(bw->w, bw->height); /* Image height */
	writeShort(bw->w, 1); /* Bit-planes */
	writeShort(bw->w, 24); /* Colour depth */
	writeLong(bw->w, 0); /* Uncompressed RGB mode */
	writeLong(bw->w, byteWidth(bw)*bw->height); /* Bitmap size */
	writeLong(bw->w, 0); /* Horizontal resolution */
	writeLong(bw->w, 0); /* Vertical resolution */
	writeLong(bw->w, 0); /* Number of colours in palette */
	writeLong(bw->w, 0); /* Ignored */
}

void writeBMP24Bitmap(struct bmp24_writer_t *bw, BYTE *pixBuf) {
	LONG row;
	LONG rowI;
	BYTE i;

	rowI = bw->height - 1;
	for (row = bw->height; row > 0; row--) {
		writeBytes(bw->w, pixBuf + rowI*bw->width*3, bw->width*3);
		for (i = 0; i < (byteWidth(bw) - bw->width*3); i++)
			writeByte(bw->w, '\0');
		rowI--;
	}
}

