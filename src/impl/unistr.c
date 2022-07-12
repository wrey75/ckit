
/**
 * UNICODE 16 bits by character.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <assert.h>
#include "../include/class.h"
#include "../include/memory.h"
#include "../include/unistr.h"

DEFINE_CLASS(UString) {
    uint16_t len;
    wchar_t *buf;
    uint16_t size;
};

END_CLASS(UString)

ALLOCATE_DEFINITION(UString)
{
    ptr->size = 1;
    ptr->buf = ckit_alloc(ptr->size * 2);
    ptr->buf[0] = 0;
    ptr->len = 0;
}

void UString_append(UString *str, const char *completion)
{
}


UString *ustr_from(const char *source)
{
    UString *str = NEW(UString);
    ustr_concat(str, source);
    return str;
}

size_t ustr_length(const UString *str) {
    return str->len;
}


static void ustr_ensure(UString *str, int requested){
    if(requested >= str->size){
        size_t newlen = str->size < 2000 ? str->size * sizeof(wchar_t) : str->len + 2000;
        if(requested >= newlen) newlen = requested + 16;
        str->buf = ckit_realloc(str->buf, newlen * sizeof(wchar_t));
        str->size = newlen;
    }
}

void ustr_concat(UString*str, const char *source){
    ustr_ensure(str, ustr_length(str) + strlen(source));
    size_t copied = mbstowcs( &str->buf[str->len], source, strlen(source)+1 );
    str->len = str->len + copied;
    assert(str->buf[str->len] == 0);
}

void ustr_rtrim(UString *str){
    int i = str->len;
    while(i > 0 && iswspace(str->buf[i - 1])) i--;
    str->len = i;
    str->buf[str->len] = 0;
}

void ustr_ltrim(UString *str){
    int i = 0;
    while( i < str->len && iswspace(str->buf[i])) i++;
    if(i > 0){
        memmove(str->buf, &str->buf[i], str->len - i + 1);
    }
    assert(str->buf[str->len] == 0);
}


void ustr_trim(UString *self){
    ustr_rtrim(self);
    ustr_ltrim(self);
}
