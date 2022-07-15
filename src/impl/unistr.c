
/**
 * UNICODE 16 bits by character.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <assert.h>
#include <wctype.h>
#include "../include/class.h"
#include "../include/memory.h"
#include "../include/unistr.h"

DEFINE_CLASS(UString) {
    uint16_t len;
    wchar_t *buf;
    uint16_t size;
};

#define WCHAR_SIZE siezof(wchar_t)

ALLOCATE_DEFINITION(UString)
{
    self->size = 1;
    self->buf = ckit_alloc(sizeof(wchar_t));
    self->buf[0] = 0;
    self->len = 0;
}

DISPOSE_DEFINITION(UString) {
    ckit_free(self->buf);
}

REGISTER_CLASS(UString)


UString *ustr_from(const char *source)
{
    UString *str = NEW(UString);
    ustr_concat(str, source);
    return str;
}

size_t ustr_length(const UString *str) {
    return str->len;
}

const wchar_t *ustr_buffer(const UString *str){
    return (const wchar_t *)str->buf;
}

static void ustr_ensure(UString *str, int requested){
    if(requested >= str->size){
        size_t newlen = str->size < 2000 ? str->size : str->len + 2000;
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
        memmove(str->buf, &str->buf[i], (str->len - i + 1) * sizeof(wchar_t));
        str->len -= i; 
    }
    assert(str->buf[str->len] == 0);
}


void ustr_trim(UString *self){
    ustr_rtrim(self);
    ustr_ltrim(self);
}
