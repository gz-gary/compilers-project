#include "log.h"
#include <stdio.h>

extern int parse_error;

inline static void log_error_type(const char *error_type) {
    parse_error = 1;
    fprintf(stdout, "Error type %s", error_type);
}

inline static void log_error_lineno(int error_lineno) {
    fprintf(stdout, "at Line %d", error_lineno);
}

inline static void log_error_type_lineno(const char *error_type, int error_lineno) {
    log_error_type(error_type);
    fprintf(stdout, " ");
    log_error_lineno(error_lineno);
    fprintf(stdout, ": ");
}

void log_mystery_char_error(char chr, int lineno) {
    log_error_type_lineno("A", lineno);
    fprintf(stdout, "Mysterious characters \'%c\'", chr);
    fprintf(stdout, "\n");
}

void log_invalid_token_error(const char *token, int lineno) {
    log_error_type_lineno("A", lineno);
    fprintf(stdout, "invalid token \'%s\'", token);
    fprintf(stdout, "\n");
}

void yyerror(const char *s) {
    log_error_type("B");
    fprintf(stdout, ": ");
    fprintf(stdout, "%s", s);
    fprintf(stdout, "\n");
}