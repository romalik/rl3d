#ifndef CORE_H_
#define CORE_H
#include "config.h"
#include "types.h"
#include "io_generic.h"

int core_init(int argc, char ** argv);
int core_loop();
int core_uninit();

#endif
