
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
#include "../ckit.h"

DEFINE_CLASS(String) {
    wchar_t *wbuf;  // the buffer in UNICODE
    unsigned len : 16; // the length of the buffer
    unsigned size : 20; // size in characters of the string (up to 65KB)
};

#define MAX_LENGTH 16000000

ALLOCATE_DEFINITION(String)
{
    self->wbuf = ckit_alloc(sizeof(wchar_t));
    self->wbuf[0] = 0;
    self->len = 0;
    self->size = 1;
}

DISPOSE_DEFINITION(String) {
    ckit_free(self->wbuf);
}

REGISTER_CLASS(String)


static void str_ensure_size(String *self, size_t requested){
    if(requested > self->size){
        if(requested > MAX_LENGTH){
            char msg[100];
            sprintf(msg, "A string can not be larger than %i bytes (request %zu).", MAX_LENGTH, requested);
            ckit_exit(msg);
        }
        size_t newsize = self->size < 2000 ? self->size * 2 : self->len + 2000;
        if(requested > newsize) newsize = requested + 16;
        self->wbuf = ckit_realloc(self->wbuf, newsize * sizeof(wchar_t));
        self->size = newsize;
    }
}


String *str_from(const char *source)
{
    String *str = NEW(String);
    str_strcat(str, source);
    return str;
}

void str_strcat(String *self, const char *source){
    if(source != NULL){
        size_t len = strlen(source);
        str_ensure_size(self, self->len + len + 1);
        mbstowcs(&self->wbuf[self->len], source, len+1);
        self->len += len;
        assert(self->wbuf[self->len] == 0);
    }
}

size_t str_length(const String *str) {
    return str->len;
}

const wchar_t *ustr_wbuffer(const String *str){
    return str->wbuf;
}


void str_wcscat(String*self, const wchar_t *source){
    if(source != NULL){
        size_t len = wcslen(source);
        str_ensure_size(self, self->len + len + 1);
        wcscpy(&self->wbuf[self->len], source);
        self->len += len;
        assert(self->wbuf[self->len] == 0);
    }
}

void str_rtrim(String *self){
    int i = self->len;
    while(i > 0 && iswspace(self->wbuf[i - 1])) i--;
    self->len = i;
    self->wbuf[self->len] = 0;
}

void str_ltrim(String *self){
    int i = 0;
    while( i < self->len && iswspace(self->wbuf[i])) i++;
    if(i > 0){
        memmove(self->wbuf, &self->wbuf[i], (self->len - i + 1) * sizeof(wchar_t));
        self->len -= i; 
    }
    assert(self->wbuf[self->len] == 0);
}


void str_trim(String *self){
    str_rtrim(self);
    str_ltrim(self);
}

void str_char_at(String *self, int pos) {
    if(pos < 0){

    } else {
        if(pos > self->len) ckit_exit("Index out of bounds");
        return self->wbuf[pos];
    }
}
