#ifndef _IO_H
#define _IO_H

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "mem.h"

struct writer_t {
	FILE *f;
	BYTE byteOrder;
};

#define BYTEORDER_BE	0
#define BYTEORDER_LE	1

struct writer_t *newWriter(FILE *);
void deleteWriter(struct writer_t *);
void writeByte(struct writer_t *, BYTE);
void writeShort(struct writer_t *, SHORT);
void writeLong(struct writer_t *, LONG);
void writeBytes(struct writer_t *, BYTE *, LONG);
void setWriterByteOrder(struct writer_t *, BYTE);

struct reader_t {
	FILE *f;
	BYTE left;
	LONG leftSz;
};

struct reader_t *newReader(FILE *);
void deleteReader(struct reader_t *);
LONG readBit(struct reader_t *);
void seekReader(struct reader_t *, LONG);
LONG readBits(struct reader_t *, LONG);
BYTE readByte(struct reader_t *);
void readBytes(struct reader_t *, BYTE *, LONG);
SHORT readShort(struct reader_t *);
LONG readLong(struct reader_t *);

#endif
