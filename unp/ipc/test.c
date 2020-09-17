#include "unpipc.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void test_px_ipc_name();

int main()
{
	printf("%s [begin]\n", __func__);

	test_px_ipc_name();

	printf("%s [end]\n", __func__);

	return 0;
}

void test_px_ipc_name()
{
	printf("%s [begin]\n", __func__);

	char *env_test = "PX_IPC_NAME";
	char *env_put = "PX_IPC_NAME=tmpname";
	char *env_old;
	char *name;
	char *name_full;

	env_old = getenv(env_test);
	printf("%s old env:%s\n", __func__, env_old);

	if (0 != putenv(env_put)) {
		printf("%s putenv error:%s\n", __func__, strerror(errno));
		return;
	}
	
	name = "fromenv";
	name_full = px_ipc_name(name);
	printf("%s from env:%s\n", __func__, name_full);
	if (NULL != name_full) {
		free(name_full);
		name_full = NULL;
	}

	if (0 != unsetenv(env_test)) {
		printf("%s unsetenv error:%s\n", __func__, strerror(errno));
		return;
	}
	
#ifdef POSIX_IPC_PREFIX
	name = "frommacro";
#else
	name = "fromdefault";
#endif
	name_full = px_ipc_name(name);
	printf("%s from non-env:%s\n", __func__, name_full);
	if (NULL != name_full) {
		free(name_full);
		name_full = NULL;
	}

	if (0 != setenv(env_test, env_old, 1)) {
		printf("%s setenv error:%s\n", __func__, strerror(errno));
		return;
	}
	printf("%s old env:%s\n", __func__, env_old);

	printf("%s [end]\n", __func__);
}
