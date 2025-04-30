#include "symbtable.h"
#include <stdio.h>
#include <stdlib.h>

void symbtable_add_entry(const char *symb, enum symb_type_t symb_type, type_t *type, int depth) {
    symbtable_entry_t *entry = malloc(sizeof(symbtable_entry_t));

    entry->symb = symb;
    entry->symb_type = symb_type;
    entry->type = type;
    entry->depth = depth;
    // printf("%s -> ", symb);
    // printf("[%s] ", symb_type == SYMB_VAR ? "variable" : (symb_type == SYMB_STRUCT ? "struct" : (symb_type == SYMB_FUNC ? "function" : "field")));
    // log_type(type);
    // printf("\n");

    hashtable_init_entry(&(entry->hashtable_entry), symb);
    hashtable_add_entry(&(entry->hashtable_entry));
}

symbtable_entry_t *symbtable_query_entry(const char *symb) {
    hashtable_entry_t *entry = hashtable_query_entry(symb);
    if (entry) return hash2symb(entry);
    else return NULL;
}

void symbtable_remove_head(const char *symb) {
    hashtable_remove_head(symb);
}
