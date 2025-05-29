#include <time.h>
#include <stdio.h>
#include "IR_parse.h"

int main(int argc, char *argv[]) {

    FILE *f = NULL;
    if (argc > 1) {
        if (!(f = fopen(argv[1], "r"))) {
            fprintf(stderr, "Error: Cannot open file %s\n", argv[1]);
            return 1;
        }
        IR_yyrestart(f);
    }
    IR_yyparse();
    // srand(time(NULL));
    // IR_parse(argc >= 2 ? argv[1] : NULL);
    // IR_output(argc >= 3 ? argv[2] : NULL);
    // if(ir_program_global != NULL)
    //     RDELETE(IR_program, ir_program_global);
    return 0;
}
