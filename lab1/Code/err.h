#ifndef ERR_H
#define ERR_H

typedef enum {
    ERR_A_MYSTERY = 0,
    ERR_A_LEXICAL,
} errA_t;

void err_A(const char *s, int lineno, errA_t err);

// TODO: add more error types
void err_B(const char *s, int lineno);

#endif