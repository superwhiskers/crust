#include <stdio.h>

#include "crust.h"

int main() {
	struct Result res;
	res.type = ResultErr;
	int i = is_err(res);
	printf("%d", i);

	res.type = ResultOk;
	int j = is_ok(res);
	printf("%d", j);
	
	if (is_ok(res)) {
		printf("OK\n");
	}
}
