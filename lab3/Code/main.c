#include <stdio.h>
#include <assert.h>
#include "semantics.h"

extern int yyparse();
extern int yyrestart(FILE *);

int parse_error = 0;

int main(int argc, char const *argv[]) {
    FILE *f = NULL;
    if (argc > 1) {
        if (!(f = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
        }
        yyrestart(f);
    }
    yyparse();
    if (!parse_error) {
        semantics_check();
    }
    return 0;
}
