#ifndef HASHTABLE_H
#define HASHTABLE_H

struct hashtable_entry_t {
    const char *key;
    struct hashtable_entry_t *next;
};
typedef struct hashtable_entry_t hashtable_entry_t;

void hashtable_init_entry(hashtable_entry_t *entry, const char *key);
unsigned int hashtable_add_entry(hashtable_entry_t *entry);
void hashtable_remove_entry(unsigned int slot_idx);
void hashtable_remove_head(const char *key);
hashtable_entry_t* hashtable_query_entry(const char *key);

#endif
