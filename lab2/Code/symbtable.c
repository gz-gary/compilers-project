#include "symbtable.h"
#include <stdlib.h>

void symbtable_add_entry(const char *symb) {
    symbtable_entry_t *entry = malloc(sizeof(symbtable_entry_t));

    entry->symb = symb;

    hashtable_init_entry(&(entry->hashtable_entry), symb);
    hashtable_add_entry(&(entry->hashtable_entry));
}

symbtable_entry_t *symbtable_query_entry(const char *symb) {
    hashtable_entry_t *entry = hashtable_query_entry(symb);
    if (entry) return hash2symb(entry);
    else return NULL;
}
