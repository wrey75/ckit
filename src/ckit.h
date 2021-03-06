/**
 * ckit.h
 * 
 * This include is to be used to load the CKit.
 */

#ifndef NDEBUG
#define CKIT_DEBUG
#endif

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "include/memory.h"
#include "include/fastarray.h"
#include "include/class.h"
#include "include/types.h"
#include "include/unistr.h"
#include "include/array.h"
#include "include/hashtable.h"
#include "include/md5.h"
#include "include/cmemory.h"
#include "include/base64.h"

extern void ckit_init();
extern void ckit_infos(FILE *);
extern void ckit_exit(const char *message);

extern void ckit_object_list(FILE *f);
extern void ckit_backtrace(void);
