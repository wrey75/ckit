
#ifndef _UNISTR_H__
#define _UNISTR_H__


CLASS_DECLARE(String)

/**
 * @brief create a UNICODE string from a basic string.
 * @param source the text source
 * 
 * @return UString* a new string 
 */
extern String* str_from(const char *source);
extern void str_strcat(String *, const char *);
extern void str_wcscat(String *, const wchar_t *);
extern void str_add(String *, const String *);
extern void str_trim(String *);
extern void str_rtrim(String *);
extern void str_ltrim(String *);
extern void str_char_at(String *, int);
extern size_t str_length(const String *str);
extern const wchar_t *ustr_buffer(const String *str);
#endif