
#ifndef _UNISTR_H__
#define _UNISTR_H__


CLASS_DECLARE(UString)

/**
 * @brief create a UNICODE string from a basic string.
 * @param source the text source
 * 
 * @return UString* a new string 
 */
extern UString* ustr_from(const char *source);
extern void ustr_concat(UString *, const char *);
extern void ustr_add(UString *, const UString *);
extern void ustr_trim(UString *);
extern void ustr_rtrim(UString *);
extern void ustr_ltrim(UString *);
extern size_t ustr_length(const UString *str);
extern const wchar_t *ustr_buffer(const UString *str);
#endif