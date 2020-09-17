#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLINE (8092)

int main()
{
	char line[MAXLINE] = { 0 };
	FILE *fpin;
	char *result = "result:";
	strncpy(line, result, strlen(result));

    if ((fpin = popen("ps", "r")) == NULL) {
		printf("error: popen\n");
	}

	while (1) {
		fputs("prompt> ", stdout);
		fflush(stdout);
		if (fgets(line + strlen(result), MAXLINE, fpin) == NULL) {
			break;
		}
		if (fputs(line, stdout) == EOF) {
			printf("error: fputs\n");
		}
	}

	if (pclose(fpin) == -1) {
		printf("error: pclose\n");
	}

	putchar('\n');

	exit(0);
}
