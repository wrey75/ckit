/**
 * ckit.h
 * 
 * This include is to be used to load the CKit.
 */

#ifndef NDEBUG
#define CKIT_DEBUG
#endif

#include <stddef.h>
#include "include/memory.h"
#include "include/class.h"
#include "include/types.h"
#include "include/unistr.h"


extern void ckit_init();
extern void ckit_infos(FILE *);
extern void ckit_exit(const char *message);
