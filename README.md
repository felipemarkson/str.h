# str.h
String builder and view library

## How to use

Add `#define STR_H_IMPLEMENTATION` before you include [`str.h`](str.h) in _**one**_ C or C++ file to create the implementation.

## Example

It should look like this (including other options):
```C
/* #define STR_H_NOSTDLIB */     /* To disable macros that use stdlib malloc/free */
#define STR_H_IMPLEMENTATION     /* To define the implementation */
#define STR_INIT_CAP 16          /* To define default initial capacity of StrB. Need STR_H_IMPLEMENTATION */
#define STR_INTERNAL_ASSERT(...) /* To avoid internal assertions. Need STR_H_IMPLEMENTATION */
#include "str.h"

#include <assert.h>
int main(void) {
    const char* input    = "Ok 2 a a a 3";
    const char* expected = "Ok2aaa3";
    char        output[8];

    StrV sv = {0};
    StrB sb = {0};

    sv = STRV(input);

    while (sv.size > 0) {
        StrV sv_temp = strv_chop(&sv, STRV(" "));
        STRB_APPENDV(&sb, sv_temp);
    }
    strb_build(output, &sb);

    assert(strcmp(expected, output) == 0);
    return 0;
}
```
