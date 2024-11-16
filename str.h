/* str - string/view/builder library; no warranty implied; use at your own risk

   Do this:
      #define STR_H_IMPLEMENTATION
   before you include this file in *one* C or C++ file to create the
implementation.

   // i.e. it should look like this:
   #include ...
   #include ...
   #include ...
   #define STR_H_IMPLEMENTATION
   #include "str.h"

MIT License
Copyright (c) 2024 Felipe M. S. Monteiro

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef STR_H
#define STR_H

#include <stddef.h> /* size_t */
#include <string.h> /* strlen */

#ifdef __cplusplus
extern "C" {
#endif

/* TYPES */
typedef char StrCh; /* TODO: UTF-32 */

typedef void* (*StrMalloc_f)(size_t);
typedef void (*StrFree_f)(void*);
typedef struct StrV {
    size_t      size;
    const char* data;
} StrV;
typedef struct StrB {
    size_t size;
    size_t capacity;
    StrV*  data;
} StrB;

/* Push back a StrV pointer into the StrBuilder.
 * The StrV pointer must while StrB is valid.
 * On malloc error, returns -1 and dest is not modified.
 * On success, returns 0 */
int strb_appendv(StrB dest[1], StrV src, StrMalloc_f m);
/* Push back a string into the StrBuilder.
 * Thestring must while StrB is valid.
 * On malloc error, returns -1 and dest is not modified.
 * On success, returns 0 */
int strb_append(StrB dest[1], const char* src, size_t n, StrMalloc_f m);
/* Free the memory used by StrB and reset Strb */
void strb_free(StrB strb[1], StrFree_f f);
/* Create a null-terminated string from a StrBuilder into char*.
 * The dest pointer must to have enought memory.
 * The memory need can be get using strb_nbytes */
void strb_build(char dest[], const StrB src[1]);
/* Allocate a new StrBuilder by copying a StrBuilder
 * Returns -1 on allocation error. */
int strb_clone(StrB dest[1], StrB str, StrMalloc_f m);

#ifndef STR_H_NOSTDLIB
#    include <stdlib.h>
#    define STRB_APPENDV(dest, src) strb_appendv(dest, src, malloc)
#    define STRB_APPEND(dest, src)  strb_append(dest, src, strlen(src), malloc)
#    define STRB_FREE(strb)         strb_free(strb, free)
#    define STRB_CLONE(dest, strb)  strb_clone(dest, strb, malloc)
#endif

/* Create a StrV based on a cstr */
StrV strv(const char* src, size_t n);
/* Copy the data from a StrV into dest. The user must to allocate enought
 * memory. The memory needed is (StrV::size + 1) bytes. */
void strv_copy(char dest[], StrV src);
/* Converts from a null-terminated `const char*` to Strv */
#define STRV(cstr) strv(cstr, strlen(cstr))

/********** INFO **********/

/* Returns the number of bytes needed to allocate a null-terminated string from
 * a StrBuilder */
size_t strb_nbytes(const StrB strb[1]);
/* Finds the first ocurrence of needle in haystack. SIZE_MAX if not found. */
size_t strv_find_first(StrV haystack, StrV needle);
/* Safety access str and return the nth StrCh in a StrV. Returns '\0' if n is
 * greater than the size of the Str */
StrCh strv_at(StrV str, size_t n);
/* Safety access str and return the first StrCh in a StrV. Returns '\0' if StrV
 * is empty */
StrCh strv_first(StrV str);
/* Safety access str and return the last StrCh in a StrV. Returns '\0' if StrV
 * is empty */
StrCh strv_last(StrV str);
/* Counts the number of StrCh in a StrV. For now, same as `StrV::size` */
size_t strv_nch(StrV str);

/*********** VIEW MANIPULATION ***********/

/* Remove the first StrCh from a StrV and returns it. Returns '\0' if empty */
StrCh strv_pop_first(StrV str[1]);
/* Remove the last StrCh from a StrV and returns it. Returns '\0' if empty */
StrCh strv_pop_last(StrV str[1]);
/* Remove the n first StrCh's from a StrV and returns it. Returns number of
 * StrCh removed */
size_t strv_remove_front(StrV base[1], size_t n);
/* Remove the n last StrCh's from a StrV and returns it. Returns number of StrCh
 * removed */
size_t strv_remove_back(StrV base[1], size_t n);
/* Create a new StrV with the n first StrCh's from a `base` and remove the n
 * first StrCh's from `base` */
StrV strv_split_at(StrV base[1], size_t n);
/* Create a new StrV from begining of `base` until the first occurrence of
 * `delim`. `delim` will not be in the new StrV. All StrCh from the new StrV
 * plus delim is removed from `base`. If `delim` is not found in `base`, the new
 * StrV will be equal to `base` and `base` will be set to empty */
StrV strv_chop(StrV base[1], StrV delim);

/********** VIEW COMPARATION **********/

/* Compare two StrV. Same behaviour of `strlen` */
int strv_cmp(StrV one, StrV other);

/********** PRINT UTILS **********/

/* HOW TO USE:
 * E.g:
 *      printf("My StrV is: '" STRV_FMT "'\n", STRV_PARAM(strv))
 */
#define STRV_FMT        "%.*s"
#define STRV_PARAM(stv) (int)((stv).size), ((stv).data)

#ifdef __cplusplus
}
#endif

/********** STR_H_IMPLEMENTATION **********/
#ifdef STR_H_IMPLEMENTATION

#    ifdef SIZE_MAX
#        define STR_SIZE_MAX SIZE_MAX
#    else
#        define STR_SIZE_MAX ((size_t) - 1)
#    endif

#    ifndef STR_H_INIT_CAP
#        define STR_H_INIT_CAP 8
#    endif

#    ifndef STR_INTERNAL_ASSERT
#        include <assert.h>
#        define STR_INTERNAL_ASSERT(expr) assert(expr)
#    endif

#    include <string.h> /* memcpy, memset, strncmp */

void strb_free(StrB strb[1], StrFree_f f) {
    f(strb->data);
    strb->data     = NULL;
    strb->capacity = 0;
    strb->size     = 0;
}

int strb_appendv(StrB dest[1], StrV src, StrMalloc_f m) {
    if (dest->size >= dest->capacity) {
        void*  temp = NULL;
        size_t size = sizeof(*(dest->data));
        size_t cap_needed =
            dest->capacity > 0 ? dest->capacity * 2 : STR_H_INIT_CAP;

        temp = m(cap_needed * size);
        if (temp == NULL)
            return -1;

        if (dest->size > 0) {
            STR_INTERNAL_ASSERT(dest->data);
            memcpy(temp, dest->data, dest->size * size);
        }

        memset((char*)temp + dest->size * size,
               0,
               (cap_needed - dest->size) * size);
        dest->data     = temp;
        dest->capacity = cap_needed;
    }
    if (src.size > 0) {
        dest->data[dest->size] = src;
        dest->size += 1;
    }
    return 0;
}

int strb_append(StrB dest[1], const char* src, size_t n, StrMalloc_f m) {
    StrV sv;
    sv.size = n;
    sv.data = src;
    return strb_appendv(dest, sv, m);
}

void strb_build(char dest[], const StrB src[1]) {
    size_t i;
    for (i = 0; i < src->size; ++i) {
        StrV   strv   = src->data[i];
        size_t nbytes = strv.size * sizeof(*(strv.data));
        memcpy(dest, strv.data, nbytes);
        dest += nbytes;
    }
    *dest = '\0';
}

StrV strv(const char* src, size_t n) {
    StrV out;
    out.data = src;
    if (src == NULL)
        out.size = 0;
    else
        out.size = n;
    return out;
}

size_t strv_find_first(StrV haystack, StrV needle) {
    size_t i = 0;

    int empty_haystack = haystack.size == 0;
    int empty_needle   = needle.size == 0;

    if (empty_haystack && empty_needle)
        return 0;
    else if (empty_haystack && !empty_needle)
        return 0;
    else if (!empty_haystack && empty_needle)
        return STR_SIZE_MAX;
    else /* NEITHER EMPTY */
    { }  /* DO NOTHING */

    if (haystack.size < needle.size)
        return STR_SIZE_MAX;

    for (i = 0; i <= haystack.size - needle.size; i++)
        if (strncmp(haystack.data + i,
                    needle.data,
                    needle.size * sizeof(*needle.data)) == 0)
            return i;
    return STR_SIZE_MAX;
}

void strv_copy(char dest[], StrV src) {
    memcpy(dest, src.data, src.size);
    dest[src.size] = '\0';
}

/* TODO: UTF-32 */
StrCh strv_at(StrV str, size_t n) {
    if (n < str.size)
        return (StrCh)str.data[n];
    return 0;
}

/* TODO: UTF-32 */
StrCh strv_first(StrV str) {
    if (str.size > 0)
        return (StrCh)*str.data;
    return 0;
}

/* TODO: UTF-32 */
StrCh strv_last(StrV str) {
    if (str.size > 0)
        return (StrCh)str.data[str.size - 1];
    return 0;
}

/* TODO: UTF-32 */
StrCh strv_pop_first(StrV str[1]) {
    if (str == NULL)
        return 0;
    if (str->size > 0) {
        StrCh out = *str->data;
        str->data += 1;
        str->size -= 1;
        if (str->size == 0)
            str->data = NULL;
        return out;
    }
    return 0;
}

/* TODO: UTF-32 */
StrCh strv_pop_last(StrV str[1]) {
    if (str == NULL)
        return 0;
    if (str->size > 0) {
        StrCh out = 0;
        str->size -= 1;
        out = str->data[str->size];
        if (str->size == 0)
            str->data = NULL;
        return out;
    }
    return 0;
}

/* TODO: UTF-32 */
size_t strv_nch(StrV str) { return str.size; }

StrV strv_split_at(StrV base[1], size_t n) {
    StrV newv = {0};
    if (n > base->size)
        n = base->size;
    newv.data = base->data;
    newv.size = n;
    base->data += n;
    base->size = base->size - n;
    if (base->size == 0)
        base->data = NULL;
    return newv;
}

StrV strv_chop(StrV base[1], StrV delim) {
    size_t loc = STR_SIZE_MAX;
    StrV   out = {0};

    loc = strv_find_first(*base, delim);
    out = strv_split_at(base, loc);
    if (base->size == 0) {
        base->data = NULL;
        return out;
    }

    base->data += delim.size;
    base->size -= delim.size;
    return out;
}

size_t strv_remove_front(StrV base[1], size_t n) {
    if (n > base->size)
        n = base->size;

    base->data += n;
    base->size -= n;
    if (base->size == 0)
        base->data = NULL;
    return n;
}

size_t strv_remove_back(StrV base[1], size_t n) {
    if (n > base->size)
        n = base->size;

    base->size -= n;
    if (base->size == 0)
        base->data = NULL;
    return n;
}

int strv_cmp(StrV one, StrV other) {
    int empty_one   = one.size == 0;
    int empty_other = other.size == 0;

    if (empty_one && empty_other)
        return 0;
    else if (empty_one && !empty_other)
        return 1;
    else if (!empty_one && empty_other)
        return -1;
    else /* NEITHER EMPTY */
        if (one.data && other.data)
            if (one.size == other.size)
                return strncmp(one.data, other.data, one.size);
            else {
                size_t   i    = 0;
                long int acc1 = 0;
                long int acc2 = 0;
                for (i = 0; i < one.size; i++)
                    acc1 += one.data[i];
                for (i = 0; i < other.size; i++)
                    acc2 += other.data[i];
                return (int)(acc1 - acc2);
            }
        else
            return -1; /* UNREACHABLE */
}
size_t strb_nbytes(const StrB strb[1]) {
    size_t i      = 0;
    size_t nbytes = 1;
    for (; i < strb->size; ++i)
        nbytes += strb->data[i].size * sizeof(*(strb->data));
    return nbytes;
}

#endif /* STR_H_IMPLEMENTATION */

#endif /* STR_H */
