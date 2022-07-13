
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../ckit.h"

#define RUN(a) \
    fprintf(stdout, "Running %s...", #a); \
    a(); \
    fprintf(stdout, " OK.\n"); 

#define STEP fprintf(stdout, ".")

void test1() {
    UString *str = NEW(UString);
    assert(ustr_length(str) == 0);
    STEP;
    ustr_concat(str, "  Le chat est mort. ");
    assert(ustr_length(str) == strlen("  Le chat est mort. "));
    STEP;
    ustr_trim(str);
    //  printf("[%ls] = %zu chars\n", ustr_buffer(str), ustr_length(str));
    assert(ustr_length(str) == strlen("Le chat est mort."));
    DEL(str);
}

int main(int argc, const char *argv[]){
    ckit_init();
    ckit_infos(stdout);
    RUN(test1);
    ckit_infos(stdout);
    ckit_object_list(stderr);
}