#include <stdio.h>

extern FILE* yyin;
extern int yyparse();

int main(int argc, char const *argv[]) {
    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
        }
    }
    yyparse();
    return 0;
}
