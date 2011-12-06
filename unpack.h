#include "io.h"
#include "mem.h"

#include <stdlib.h>
#include <string.h>

struct entry_info_t {
	LONG offset;
	LONG size;
	LONG type;
	LONG id;
};

#define TYPE_PDf5	0x50446635
#define TYPE_Bit2	0x42697432
#define TYPE_Clrs	0x436c7273

typedef void (*UnpackCallback)(LONG, LONG, LONG, BYTE *);
