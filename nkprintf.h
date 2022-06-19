#include <stdio.h>
#include <stdarg.h>

void nkprint(const char *);

static void nkprintf(const char *fmt, ...) {
	va_list vargs;
	char buf[512];
	va_start(vargs, fmt);
	vsprintf(buf, fmt, vargs);
	nkprint(buf);
}
