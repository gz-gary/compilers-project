#include "log.h"
#include <stdio.h>

extern int parse_error;
extern int yylineno;

inline static void log_error_type(const char *error_type) {
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

extern int lexerr_thisline;
void yyerror(const char *s) {
    parse_error = 1;
    if (lexerr_thisline) return;
    log_error_type("B");
    fprintf(stdout, " ");
    log_error_lineno(yylineno);
    fprintf(stdout, ": ");
    fprintf(stdout, "%s", s);
    fprintf(stdout, "\n");
}

void log_semantics_error_prologue(const char *errid, int lineno) {
    log_error_type_lineno(errid, lineno);
}