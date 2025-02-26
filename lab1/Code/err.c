#include "err.h"
#include <stdio.h>

void err_A(const char *s, int lineno, errA_t err) {
    if (err == ERR_A_MYSTERY) {
        printf("Error type A at Line %d: Mysterious characters '%s'\n", lineno, s);
    } else if (err == ERR_A_LEXICAL) {
        printf("Error type A at Line %d: invalid token '%s'\n", lineno, s);
    }
}