#include "str.h"

#include <assert.h>
#include <stdio.h>

#define STR_ASSERT(expr, ...)                                                  \
    do {                                                                       \
        if (!(expr)) {                                                         \
            fprintf(stderr, "%s:%d : ", __FILE__, __LINE__);                   \
            fprintf(stderr, __VA_ARGS__);                                      \
            fflush(stderr);                                                    \
            assert(0 && "ASSERTION FAIL!");                                    \
        }                                                                      \
    } while (0)

#define STRV_ASSERTEQ(one, other, msg)                                         \
    STR_ASSERT(strv_cmp(one, other) == 0,                                      \
               msg "\n    One:   '" STRV_FMT "'\n"                             \
                   "    Other: '" STRV_FMT "'\n",                              \
               STRV_PARAM(one),                                                \
               STRV_PARAM(other))

#define STRV_ASSERTNEQ(one, other, msg)                                        \
    STR_ASSERT(strv_cmp(one, other) != 0,                                      \
               msg "\n   One:   '" STRV_FMT "'\n"                              \
                   "   Other: '" STRV_FMT "'\n",                               \
               STRV_PARAM(one),                                                \
               STRV_PARAM(other))

void test_cmp(void) {
    STRV_ASSERTEQ(
        STRV("1 2 3"), STRV("1 2 3"), "strv_cmp must return 0 on equal StrV");

    STRV_ASSERTNEQ(STRV("1 2 3"),
                   STRV("1 2 "),
                   "strv_cmp must NOT return 0 on NOT equal StrV");

    STRV_ASSERTNEQ(STRV("1 2 3"),
                   STRV("1 2 3 4"),
                   "strv_cmp must NOT return 0 on NOT equal StrV");

    STRV_ASSERTNEQ(STRV("1 2 3 4"),
                   STRV(" 2 3 4"),
                   "strv_cmp must NOT return 0 on NOT equal StrV");
}

void test_strb_append(void) {
    StrB sb = {0};
    STRB_APPEND(&sb, "TEST");
    STR_ASSERT(sb.size == 1, "StrB should not be empty after STRB_APPEND");

    char data[5];
    strb_build(data, &sb);
    STR_ASSERT(strcmp(data, "TEST") == 0, "Out of strb_build should be equal");
    STRB_FREE(&sb);
}
void test_strb_append_empty(void) {
    StrB sb = {0};
    StrV sv = {0};
    STRB_APPENDV(&sb, sv);
    STR_ASSERT(sb.size == 0, "StrB should be 0 after STRB_APPEND with empty SV");

    char data[5];
    strb_build(data, &sb);
    STR_ASSERT(strlen(data) == 0,
               "append an zero sv should create a empty cstr");
    STRB_FREE(&sb);
}
void test_strb_append_twice(void) {
    StrB sb = {0};
    StrV sv = STRV("Holla!");
    STRB_APPENDV(&sb, sv);
    STRB_APPENDV(&sb, sv);
    STR_ASSERT(sb.size == 2,
               "StrB should be equal to 2 after STRB_APPEND twice");

    char data[13];
    strb_build(data, &sb);
    STR_ASSERT(strlen(data) == 12, "out must have 12 of len");
    STR_ASSERT(strcmp(data, "Holla!Holla!") == 0, "out do not match");
    STRB_FREE(&sb);
}
void test_build_empty(void) {
    StrB input = {0};
    char output[1];
    strb_build(output, &input);
    STR_ASSERT(strlen(output) == 0, "Empty StrB should build an empty cstr");
    STRB_FREE(&input);
}

void test_build_with_chopp(void) {
    const char* input    = "Ok 2 a a a 3";
    const char* expected = "Ok2aaa3";
    char        output[8];

    StrV sv = STRV(input);
    StrB sb = {0};

    while (sv.size > 0) {
        StrV sv_temp = strv_chop(&sv, STRV(" "));
        STRB_APPENDV(&sb, sv_temp);
    }
    strb_build(output, &sb);

    STR_ASSERT(strcmp(expected, output) == 0,
               "Output should be equal to expected");

    STRB_FREE(&sb);
}

int main(void) {
    test_cmp();
    test_strb_append();
    test_strb_append_empty();
    test_strb_append_twice();
    test_build_empty();
    test_build_with_chopp();
    return 0;
}
