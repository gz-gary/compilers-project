#ifndef LOG_H
#define LOG_H

void log_mystery_char_error(char chr, int lineno);
void log_invalid_token_error(const char *token, int lineno);
void log_semantics_error_prologue(const char *errid, int lineno);
/* void log_xxxxx_error(info1, info2, ... , int lineno) */

#endif