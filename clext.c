#include <errno.h>
#include <string.h>

#include "bmp.h"
#include "io.h"

int progress = 0;
void unpackCB(LONG id, LONG width, LONG height, BYTE *rgbBuf) {
	struct writer_t *w;
	struct bmp24_writer_t *bw;
	FILE *f;
	char fn[256];

	if (rgbBuf == NULL)
		return;

	sprintf(fn, "out/%d.bmp", id);

	f = fopen(fn, "wb");
	if (f == NULL) {
		fprintf(stderr, "Failed to open %s for writing: %s\n", fn, strerror(errno));
		exit(1);
	}

	w = newWriter(f);
	bw = newBMP24Writer(w, width, height);

	writeBMP24Header(bw);
	writeBMP24Bitmap(bw, rgbBuf);

	deleteBMP24Writer(bw);
	deleteWriter(w);

	fclose(f);

	if (progress % 10 == 0)	
		fprintf(stderr, "%d..\n", progress);
	progress++;
}

int main(void) {
	FILE *f;
	struct reader_t *r;

	f = fopen("CL_Images", "rb");
	if (f == NULL) {
		fprintf(stderr, "Failed to open CL_Images: %s\n", strerror(errno));
		return 1;
	}

	r = newReader(f);

	unpack(r, unpackCB);

	deleteReader(r);
	fclose(f);

	return 0;
}
