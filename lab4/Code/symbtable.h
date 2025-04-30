#ifndef SYMBTABLE_H
#define SYMBTABLE_H

#include "type.h"
#include "hashtable.h"
#define hash2symb(entry) (symbtable_entry_t *)((void *)(entry) - (void *)(&(((symbtable_entry_t *)(0))->hashtable_entry)))

enum symb_type_t { SYMB_VAR, SYMB_STRUCT, SYMB_FIELD, SYMB_FUNC };
struct symbtable_entry_t {
    const char *symb;
    enum symb_type_t symb_type;
    type_t *type;
    hashtable_entry_t hashtable_entry;
    int depth;
};
typedef struct symbtable_entry_t symbtable_entry_t;

void symbtable_add_entry(const char *symb, enum symb_type_t symb_type, type_t *type, int depth);
symbtable_entry_t* symbtable_query_entry(const char *symb);
void symbtable_remove_head(const char *symb);

#endif