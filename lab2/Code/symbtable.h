#ifndef SYMBTABLE_H
#define SYMBTABLE_H

#include "type.h"
#include "hashtable.h"
#define hash2symb(entry) (symbtable_entry_t *)((void *)(entry) - (void *)(&(((symbtable_entry_t *)(0))->hashtable_entry)))

struct symbtable_entry_t {
    const char *symb;
    type_t *type;
    hashtable_entry_t hashtable_entry;
};
typedef struct symbtable_entry_t symbtable_entry_t;

void symbtable_add_entry(const char *symb, type_t *type);
symbtable_entry_t* symbtable_query_entry(const char *symb);

#endif