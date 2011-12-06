#include "io.h"

struct writer_t *newWriter(FILE *f) {
	struct writer_t *w;

	w = (struct writer_t *)malloc2(sizeof(*w));

	w->f = f;
	w->byteOrder = BYTEORDER_BE;

	return w;
}

void setWriterByteOrder(struct writer_t *w, BYTE o) {
	w->byteOrder = o;
}

void deleteWriter(struct writer_t *w) {
	free(w);
}

void writeByte(struct writer_t *w, BYTE x) {
	if (fputc(x, w->f) == EOF) {
		fprintf(stderr, "writeByte: unexpected EOF\n");
		exit(1);
	}
}

void writeShort(struct writer_t *w, SHORT x) {
	BYTE a, b;
	if (w->byteOrder == BYTEORDER_BE) {
		a = (x >> 8) & 0xff;
		b = x & 0xff;
	} else {
		b = (x >> 8) & 0xff;
		a = x & 0xff;
	}
	writeByte(w, a);
	writeByte(w, b);
}

void writeLong(struct writer_t *w, LONG x) {
	BYTE a, b, c, d;
	if (w->byteOrder == BYTEORDER_BE) {
		a = (x >> 24) & 0xff;
		b = (x >> 16) & 0xff;
		c = (x >> 8) & 0xff;
		d = x & 0xff;
	} else {
		d = (x >> 24) & 0xff;
		c = (x >> 16) & 0xff;
		b = (x >> 8) & 0xff;
		a = x & 0xff;
	}
	writeByte(w, a);
	writeByte(w, b);
	writeByte(w, c);
	writeByte(w, d);
}

void writeBytes(struct writer_t *w, BYTE *bs, LONG n) {
	if (fwrite(bs, n, 1, w->f) != 1) {
		fprintf(stderr, "Error in writeBytes.\n");
		exit(1);
	}
}

struct reader_t *newReader(FILE *f) {
	struct reader_t *r;

	r = (struct reader_t *)malloc2(sizeof(*r));
	r->f = f;
	r->left = 0;
	r->leftSz = 0;

	return r;
}

void deleteReader(struct reader_t *r) {
	free(r);
}

LONG readBit(struct reader_t *r) {
	if (r->leftSz != 0) {
		LONG b;
		r->leftSz--;
		b = r->left & 1;
		r->left >>= 1;
		return b;
	} else {
		LONG x;
		LONG i;
		r->left = 0;
		x = readByte(r);
		for (i = 0; i < 8; i++) {
			r->left <<= 1;
			r->left |= x & 1;
			x >>= 1;
		}
		r->leftSz = 8;
		return readBit(r);
	}
}

LONG readBits(struct reader_t *r, LONG n) {
	LONG x;
	for (x = 0; n > 0; n--) {
		x <<= 1;
		x |= readBit(r);
	}

	return x;
}

void seekReader(struct reader_t *r, LONG w) {
	r->left = 0;
	r->leftSz = 0;

	if (fseek(r->f, w, SEEK_SET) == -1) {
		fprintf(stderr, "seekReader failed.\n");
		exit(1);
	}
}

BYTE readByte(struct reader_t *r) {
	int x;

	if (r->leftSz != 0) {
		fprintf(stderr, "attempted non-aligned readByte\n");
		exit(1);
	}

	x = fgetc(r->f);
	if (x == EOF) {
		fprintf(stderr, "readByte failed.\n");
		exit(1);
	}
	return x;
}

void readBytes(struct reader_t *r, BYTE *bs, LONG n) {
	if (fread(bs, n, 1, r->f) != 1) {
		fprintf(stderr, "readBytes failed.\n");
		exit(1);
	}
}

SHORT readShort(struct reader_t *r) {
	BYTE a;
	BYTE b;

	a = readByte(r);
	b = readByte(r);

	return (a << 8) | b;
}

LONG readLong(struct reader_t *r) {
	BYTE a;
	BYTE b;
	BYTE c;
	BYTE d;

	a = readByte(r);
	b = readByte(r);
	c = readByte(r);
	d = readByte(r);

	return (a << 24) | (b << 16) | (c << 8) | d;
}

