#include <stdio.h>
#include "core.h"

int main(int argc, char ** argv) {
	core_init(argc, argv);
	while(1) {
		if(core_loop())
			break;
	}
	core_uninit();
	return 0;
}
