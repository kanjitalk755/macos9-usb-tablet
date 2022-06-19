#include <stdio.h>

int counter;

void put(int chr) {
	if (counter == 0) {
		putchar('\'');
	}

	putchar(chr);
	counter++;

	if (counter == 76) {
		putchar('\'');
		putchar('\\');
		putchar('\n');
		counter = 0;
	}
}

const char sixtyFour[] = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

void base64(FILE *f) {
	while (!feof(f)) {
		unsigned long l = 0;
		int i;

		for (i = 0; i < 24; i += 8) {
			int ch = fgetc(f);
			if (ch == EOF) {
				if (i == 0) return;
				ch = 0;
			}
			l |= (unsigned long)ch << i;
		}

		for (i = 18; i >= 0; i -= 6) {
			put(sixtyFour[(l >> i) & 0x3f]);
		}
	}
}

int main(int argc, char **argv) {
	FILE *script, *binary;
	int chr;
	int comment = 0;
	int newword = 0;

	if (argc != 3) {
		printf("Incorrect arguments\n");
		return 1;
	}

	script = fopen(argv[1], "r");
	binary = fopen(argv[2], "r");

	printf("-device usb-tablet -prom-env 'use-nvramrc?=true' -prom-env nvramrc=\\\n");

	while ((chr = fgetc(script)) != EOF) {
		if (chr == '\r' || chr == '\n') comment = 0;
		if (chr == '\\' || chr == '(') comment = 1;

		if (!comment) {
			if (chr == '\r' || chr == '\n' || chr == '\t' || chr == ' ') {
				newword = 1;
			} else {
				if (newword) {
					put(' ');
					newword = 0;
				}

				if (chr == '~') {
					base64(binary);
				} else {
					put(chr);
				}
			}
		}
	}

	if (counter != 0) {
		putchar('\'');
		putchar('\n');
	}

	return 0;
}
