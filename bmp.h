#include <stdio.h>
#include <string.h>

#include "io.h"
#include "types.h"

struct bmp24_writer_t {
	struct writer_t *w;
	LONG width;
	LONG height;
};

struct bmp24_writer_t *newBMP24Writer(struct writer_t *, LONG, LONG);
void deleteBMP24Writer(struct bmp24_writer_t *);
void writeBMP24Header(struct bmp24_writer_t *);
void writeBMP24Bitmap(struct bmp24_writer_t *, BYTE *);
