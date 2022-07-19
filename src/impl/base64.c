/**
 * Original algorithm by (c) 2014 joseph werle
 */

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../ckit.h"

#define MAGIC_NUMBER "base64"

static const char b64_table[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'};

char *base64_encode(const uint8_t *src, size_t len)
{
    int i = 0;
    int j = 0;
    char *enc = NULL;
    size_t size = 0;
    unsigned char buf[4];
    unsigned char tmp[3];

    // allocate enough size
    enc = (char *)ckit_magic_alloc(4 * (len / 3) + 8, MAGIC_NUMBER);

    // parse until end of source
    while (len--)
    {
        // read up to 3 bytes at a time into `tmp'
        tmp[i++] = *(src++);

        // if 3 bytes read then encode into `buf'
        if (3 == i)
        {
            buf[0] = (tmp[0] & 0xfc) >> 2;
            buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
            buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
            buf[3] = tmp[2] & 0x3f;

            // allocate 4 new byts for `enc` and
            // then translate each encoded buffer
            // part by index from the base 64 index table
            // into `enc' unsigned char array
            for (i = 0; i < 4; ++i)
            {
                enc[size++] = b64_table[buf[i]];
            }

            // reset index
            i = 0;
        }
    }

    // remainder
    if (i > 0)
    {
        // fill `tmp' with `\0' at most 3 times
        for (j = i; j < 3; ++j)
        {
            tmp[j] = '\0';
        }

        // perform same codec as above
        buf[0] = (tmp[0] & 0xfc) >> 2;
        buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
        buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
        buf[3] = tmp[2] & 0x3f;

        // perform same write to `enc` with new allocation
        for (j = 0; (j < i + 1); ++j)
        {
            enc[size++] = b64_table[buf[j]];
        }

        // while there is still a remainder
        // append `=' to `enc'
        while ((i++ < 3))
        {
            enc[size++] = '=';
        }
    }

    enc[size] = '\0';
    return enc;
}

/**
 * @brief decode the base 64 to a buffer having enough space
 *
 * @param src the source
 * @param dec must be at least of the same size than the 'src'.
 * @return size_t the decoded size
 */
size_t b64_decode_ex(const char *src, uint8_t *dec)
{
    int i = 0;
    int j = 0;
    int l = 0;
    size_t size = 0;
    unsigned char buf[3];
    unsigned char tmp[4];

    // parse until end of source
    size_t len = strlen(src);
    while (len--)
    {
        // break if char is `=' or not base64 char
        if ('=' == src[j])
        {
            break;
        }
        if (!(isalnum(src[j]) || '+' == src[j] || '/' == src[j]))
        {
            break;
        }

        // read up to 4 bytes at a time into `tmp'
        tmp[i++] = src[j++];

        // if 4 bytes read then decode into `buf'
        if (4 == i)
        {
            // translate values in `tmp' from table
            for (i = 0; i < 4; ++i)
            {
                // find translation char in `b64_table'
                for (l = 0; l < 64; ++l)
                {
                    if (tmp[i] == b64_table[l])
                    {
                        tmp[i] = l;
                        break;
                    }
                }
            }

            // decode
            buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
            buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
            buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];

            // write decoded buffer to `dec'
            for (i = 0; i < 3; ++i)
            {
                dec[size++] = buf[i];
            }

            // reset
            i = 0;
        }
    }

    // remainder
    if (i > 0)
    {
        // fill `tmp' with `\0' at most 4 times
        for (j = i; j < 4; ++j)
        {
            tmp[j] = '\0';
        }

        // translate remainder
        for (j = 0; j < 4; ++j)
        {
            // find translation char in `b64_table'
            for (l = 0; l < 64; ++l)
            {
                if (tmp[j] == b64_table[l])
                {
                    tmp[j] = l;
                    break;
                }
            }
        }

        // decode remainder
        buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
        buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
        buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];

        // write remainer decoded buffer to `dec'
        for (j = 0; (j < i - 1); ++j)
        {
            dec[size++] = buf[j];
        }
    }

    dec[size] = '\0';
    return size;
}

CMemory *base64_decode(const char *src)
{
    CMemory *mem = NEW(CMemory);
    // fprintf(stdout,"BEFORE:\n");
    // cmemory_dump(mem,stdout);
    uint8_t *buf = ckit_alloc(strlen(src)+1);
    size_t size = b64_decode_ex(src, buf);
    cmemory_append(mem, buf, size);
    // fprintf(stdout,"AFTER: %zu\n", size);
    // cmemory_dump(mem,stdout);
    ckit_free(buf);
    return mem;
}

void base64_free(char *encoded){
    ckit_magic_free((uint8_t *)encoded, MAGIC_NUMBER);
}