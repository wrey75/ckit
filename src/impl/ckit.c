
#include <stdio.h>
#include <stdlib.h>
#include "../ckit.h"

/**
 * Initialize the kit.
 * 
 */
void ckit_init(void) {

}

void ckit_exit(const char *message){
    fprintf(stderr, "**FATAL**: %s\n", message);
    exit(1);
}




