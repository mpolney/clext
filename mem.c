#include "mem.h"

void *malloc2(size_t sz) {
	void *p;

	p = malloc(sz);
	if (p == NULL) {
		fprintf(stderr, "malloc failure.\n");
		exit(1);
	}

	return p;
}

void *realloc2(void *p, size_t sz) {
	p = realloc(p, sz);
	if (p == NULL) {
		fprintf(stderr, "realloc failure.\n");
		exit(1);
	}

	return p;
}

