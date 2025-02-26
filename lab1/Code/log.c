#include "log.h"
#include <stdio.h>

inline static void log_error_type(const char *error_type) {
    fprintf(stderr, "Error type %s", error_type);
}

inline static void log_error_lineno(int error_lineno) {
    fprintf(stderr, "at Line %d", error_lineno);
}

inline static void log_error_type_lineno(const char *error_type, int error_lineno) {
    log_error_type(error_type);
    fprintf(stderr, " ");
    log_error_lineno(error_lineno);
    fprintf(stderr, ": ");
}

void log_mystery_char_error(char chr, int lineno) {
    log_error_type_lineno("A", lineno);
    fprintf(stderr, "Mysterious characters \'%c\'", chr);
    fprintf(stderr, "\n");
}

void log_invalid_token_error(const char *token, int lineno) {
    log_error_type_lineno("A", lineno);
    fprintf(stderr, "invalid token \'%s\'", token);
    fprintf(stderr, "\n");
}

void yyerror(const char *s) {
    log_error_type("B");
    fprintf(stderr, ": ");
    fprintf(stderr, "%s", s);
    fprintf(stderr, "\n");
}