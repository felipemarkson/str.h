#!/bin/bash
set -e
FLAGS="-Wall -Wextra -Wswitch-enum -Wswitch-default -Wconversion -Wdouble-promotion -Wformat-nonliteral -Werror -Wfatal-errors -pedantic"
gcc $FLAGS -fanalyzer -std=c89 -fsyntax-only -DSTR_H_IMPLEMENTATION str.h

rm -f str.c
printf "#define STR_H_IMPLEMENTATION\n#include \"str.h\"\n" > str.c
gcc $FLAGS -fanalyzer -std=c89 -ggdb -O0 -o str.o -c str.c
gcc $FLAGS -fanalyzer -std=c99 -ggdb -O0 -o testd test.c str.o

gcc $FLAGS -fanalyzer -std=c89 -ggdb -O3 -o str.o -c str.c
gcc $FLAGS -fanalyzer -std=c99 -ggdb -O3 -o test test.c str.o
rm -f str.c
rm str.o

