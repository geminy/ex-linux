#include "unpipc.h"
#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* px_ipc_name(const char *name)
{
	char *dst;
	char *dir;
	char *slash;

	dst = malloc(PATH_MAX);
	if (NULL == dst) {
		return NULL;
	}

	dir = getenv("PX_IPC_NAME");
	if (NULL == dir) {
#ifdef POSIX_IPC_PREFIX
		dir = POSIX_IPC_PREFIX;
#else
		dir = "/tmp/";
#endif
	}

	slash = ('/' == dir[strlen(dir) - 1]) ? "" : "/";
	snprintf(dst, PATH_MAX, "%s%s%s", dir, slash, name);

	return dst; // free from caller
}
