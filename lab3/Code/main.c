#include <stdio.h>
#include <assert.h>
#include "semantics.h"
#include "cmm2ir.h"

extern int yyparse();
extern int yyrestart(FILE *);

int parse_error = 0;

int main(int argc, char const *argv[]) {
    FILE *input = NULL;
    FILE *output = stdout;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
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
        output = fopen(argv[2], "w");
        if (!output) {
            cmm2ir();
        }
        cmm2ir_and_dump(output);
    }

    return 0;
}
