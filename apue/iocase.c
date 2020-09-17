#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main()
{
	int c;

	while ((c = getchar()) != EOF) {
		if (isupper(c)) {
			c = tolower(c);
		}
		if (putchar(c) == EOF) {
			printf("error: putchar\n");
		}
		if (c == '\n') {
			fflush(stdout);
		}

	}

	exit(0);
}
