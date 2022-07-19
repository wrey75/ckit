#ifndef _CKIT_BASE64_H__
#define _CKIT_BASE64_H__

extern CMemory *base64_decode(const char *src);

/**
 * @brief encode memory into base 64.
 * 
 * @param src the memory source
 * @param len the length in bytes
 * @return char* an allocated pointer to the string. This pointer MUST
 * be freed using the base64_free() function.
 */
extern char *base64_encode(const uint8_t *src, size_t len);

/**
 * @brief Frees the memory allocated by the call to base64_encode.
 * 
 * @param ptr 
 */
extern void base64_free(char *ptr);

#endif