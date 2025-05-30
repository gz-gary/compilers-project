#include <time.h>
#include <stdio.h>
#include "IR_parse.h"
#include "local_optimize.h"

int main(int argc, char *argv[]) {
    srand(time(NULL));
    IR_parse(argc >= 2 ? argv[1] : NULL);
    // TODO: optimize
    local_optimize();
    IR_output(argc >= 3 ? argv[2] : NULL);
    return 0;
}
