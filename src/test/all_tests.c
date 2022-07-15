
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../ckit.h"

const char *fake_pointer = "_fake";

#define RUN(a) \
    fprintf(stdout, "Running %s...", #a); \
    a(); \
    fprintf(stdout, " OK.\n"); 

#define STEP fprintf(stdout, ".")

void fastarray() {
    const char **array = ARRAY_ALLOC(const char **, 0);
    assert(ckit_array_size(array) == 0);
    STEP;

    array = ckit_array_realloc(array, 50);
    assert(ckit_array_size(array) == 50);
    for(int i = 0; i< 50; i++){
        assert(array[i] == NULL);
    }
    STEP;

    for(int i = 0; i< 50; i++){
        array[i] = fake_pointer;
    }
    array = ckit_array_realloc(array, 200);
    assert(ckit_array_size(array) == 200);
    for(int i = 0; i< 200; i++){
        if(i <50 ){
            assert(array[i] == fake_pointer);
        } else {
            assert(array[i] == NULL);
        }
    }
    STEP;

    ckit_array_free(array);
}

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

void test2() {
    Array *array = NEW(Array);
    array_add(array, (void *)38);
    array_add(array, (void *)42);
    array_add(array, (void *)59);
    assert(array_size(array) == 3);
    
    STEP;
    assert(array_get(array,1) == (void *)42);

    STEP;
    array_remove(array, 0);
    assert(array_get(array,1) == (void *)59);
    assert(array_size(array) == 2);
    DEL(array);
}

int main(int argc, const char *argv[]){
    ckit_init();
    ckit_infos(stdout);
    RUN(fastarray);
    RUN(test1);
    RUN(test2);
    ckit_infos(stdout);
    ckit_object_list(stderr);
}