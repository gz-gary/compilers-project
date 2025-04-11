#include "util.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

const char *str_new_copy(const char *origin) {
    char *copy = (char *)calloc(strlen(origin) + 1, sizeof(char));
    strcpy(copy, origin);
    return copy;
}