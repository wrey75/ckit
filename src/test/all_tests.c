
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../ckit.h"

#define RUN(a) \
    fprintf(stdout, "Running %s...", #a); \
    a(); \
    fprintf(stdout, " OK.\n"); 

#define STEP fprintf(stdout, ".")

void memory() {
    char *ptr;
    ptr = ckit_alloc(500);
    assert(*ptr != 0);
    ckit_memory_dump(stderr, ptr, 500);

    STEP;
    ckit_free(ptr);

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

void hashtable()
{
    Hashtable *hash = NEW(Hashtable);
 hash_dump(hash, stderr);
    
    hash_set(hash, "William", "shakespeare");
    hash_dump(hash, stderr);
    
    hash_set(hash, "Elphant", "man");
    hash_set(hash, "jaja", "la grande");
    hash_set(hash, "Amelie", "poulain");
    hash_set(hash, "William", "rey");
    hash_set(hash, "Thor", "Marvel");
    hash_set(hash, "Jean", "de La Fontaine");

    
 fprintf(stderr, "mmm");
    hash_set(hash, "Albert", "Londres");
    hash_set(hash, "Alberta", "simona");
    hash_set(hash, "Georges", "perec");
    hash_set(hash, "Abigail", "abigail");
    hash_set(hash, "Rossini", "rossini");
    hash_set(hash, "Alberto", "alberto");
    hash_set(hash, "Romeo", "romeo");
    hash_set(hash, "Mandelsson", "hih chart");
 
 
    hash_dump(hash, stderr);
    const char *val = hash_get(hash, "William");
   
   
    if(strcmp("rey", val) != 0) {
        printf("Erreur - found: %s\n", val);
    }

    hash_resize(hash,12);
     fprintf(stderr, "----NNNN %s", val); exit(0); 
    // display();
    
    hash_resize(hash,4);
    // display();
    DEL(hash);
}

int main(int argc, const char *argv[]){
    ckit_init();
    ckit_infos(stdout);
    RUN(hashtable);
    RUN(memory);
    RUN(test1);
    RUN(test2);
    ckit_infos(stdout);
    ckit_object_list(stderr);
}