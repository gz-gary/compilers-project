#include <stdio.h>
#include <assert.h>
#include "semantics.h"
#include "cmm2ir.h"

extern int yyparse();
extern int yyrestart(FILE *);

int parse_error = 0;
int semantics_error = 0;

int main(int argc, char const *argv[]) {
    FILE *input = NULL;
    FILE *output = stdout;

    if (argc < 2) {
        fprintf(stderr, "Usage: `%s <input_file> <output_file>` or\n", argv[0]);
        fprintf(stderr, "Usage: `%s <input_file>`, this format will output ir to stdout\n", argv[0]);
        return 1;
    }

    input = fopen(argv[1], "r");
    if (!input) {
        perror(argv[1]);
        return 1;
    }
    yyrestart(input);

    yyparse();

    if (!parse_error) {
        semantics_check();
        if (!semantics_error) {
            output = fopen(argv[2], "w");
            if (!output) {
                cmm2ir();
            }
            cmm2ir_and_dump(output);
        } else {
            fprintf(stdout, "Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
        }
    }

    return 0;
}
