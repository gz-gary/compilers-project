#include <stdio.h>

extern int yyparse();
extern int yyrestart(FILE *);
extern int yylex();

void yyerror(const char *s) {
    fprintf(stderr, "Syntax error: %s\n", s);
}

int main(int argc, char const *argv[]) {
    FILE *f = NULL;
    if (argc > 1) {
        if (!(f = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
        }
        yyrestart(f);
    }
    while (yylex() != 0);
    return 0;
}
