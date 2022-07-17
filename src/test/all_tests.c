
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../ckit.h"

const char *fake_pointer = "_fake";

#define RUN(a) \
    ckit_validate_memory(); \
    fprintf(stdout, "Running %s...", #a); \
    a(); \
    fprintf(stdout, " OK.\n"); 

#define STEP \
    ckit_validate_memory(); \
    fprintf(stdout, "."); \



void memory() {
    char *ptr;
    ptr = ckit_alloc(500);
    assert(*ptr != 0);

    ptr = ckit_realloc(ptr,550);
    assert(*ptr != 0);
    STEP;

    ptr = ckit_realloc(ptr,15);
    assert(*ptr != 0);
    
    STEP;
    ckit_free(ptr);
}

void fastarray() {
    const char **array = ARRAY_ALLOC(const char *, 0);
    assert(ckit_array_size(array) == 0);
    STEP;

    array = ckit_array_realloc(array, 50);
    assert(ckit_array_size(array) == 50);
    for(int i = 0; i< 50; i++){
        assert(array[i] == NULL);
    }
    STEP;

    for(int i = 0; i < 50; i++){
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

    array = ckit_array_realloc(array, 12);
    assert(ckit_array_size(array) == 12);
    STEP;

    ckit_array_free(array);
}

void object_UString() {
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

void object_Array() {
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

void hashtable()
{
    Hashtable *hash = NEW(Hashtable);
  
    hash_set(hash, "William", "shakespeare");
    assert(hash_size(hash) == 1);
    STEP;
    hash_set(hash, "Elphant", "man");
    assert(hash_size(hash) == 2);
    STEP;
    hash_set(hash, "jaja", "la grande");
    assert(hash_size(hash) == 3);
    STEP;
    hash_set(hash, "Amelie", "poulain");
    assert(hash_size(hash) == 4);
    STEP;
    hash_set(hash, "William", "rey"); // replace
    assert(hash_size(hash) == 4); 
    STEP;
    hash_set(hash, "Thor", "Marvel");
    assert(hash_size(hash) == 5); 
    STEP;
    hash_set(hash, "Jean", "de La Fontaine");
    assert(hash_size(hash) == 6); 
    STEP;
    hash_set(hash, "Albert", "Londres");
    assert(hash_size(hash) == 7); 
    STEP;
    hash_set(hash, "Alberta", "simona");
    assert(hash_size(hash) == 8); 
    STEP;
    hash_set(hash, "Georges", "perec");
    assert(hash_size(hash) == 9); 
    STEP;
    hash_set(hash, "Abigail", "abigail");
    assert(hash_size(hash) == 10); 
    STEP;
    hash_set(hash, "Rossini", "rossini");
    assert(hash_size(hash) == 11); 
    STEP;
    hash_set(hash, "Alberto", "alberto");
    assert(hash_size(hash) == 12); 
    STEP;
    hash_set(hash, "Romeo", "romeo");
    assert(hash_size(hash) == 13); 
    STEP;
    hash_set(hash, "Mandelsson", "hih chart");
    STEP;
 
    const char *val = hash_get(hash, "William");
    assert(strcmp("rey", val) == 0);
    STEP;

    hash_resize(hash,12);
    STEP;
    
    hash_resize(hash,4);
    const char *val1 = hash_get(hash, "William");
    assert(strcmp("rey", val1) == 0);
    STEP;

    // hash_dump(hash, stderr);
    DEL(hash);
}

int main(int argc, const char *argv[]){
    ckit_init();
    ckit_infos(stdout);
    ckit_validate_memory();

    RUN(memory);
    RUN(fastarray);
    RUN(hashtable);
    RUN(object_UString);
    RUN(object_Array);
    ckit_infos(stdout);
    ckit_object_list(stderr);
}