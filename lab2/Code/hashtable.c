#include "hashtable.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef unsigned int uint;

static hashtable_entry_t *pjw_hashtable_slot_heads[0x4000];
static inline uint pjw_hash(const char *str) {
    uint val = 0, i;
    for (; *str; ++str) {
        val = (val << 2) + *str;
        if (i = val & ~0x3fff) val = (val ^ (i >> 12)) & 0x3fff;
    }
    return val;
}

static hashtable_entry_t **hashtable_slot_heads = &pjw_hashtable_slot_heads[0];
static inline uint hashval(const char *str) {
    return pjw_hash(str);
}

void hashtable_init_entry(hashtable_entry_t *entry, const char *key) {
    entry->next = NULL;
    entry->key = key;
}

uint hashtable_add_entry(hashtable_entry_t *entry) {
    uint slot_idx = hashval(entry->key);
    if (hashtable_slot_heads[slot_idx])
        entry->next = hashtable_slot_heads[slot_idx];
    hashtable_slot_heads[slot_idx] = entry;
    return slot_idx;
}

void hashtable_remove_entry(unsigned int slot_idx) {
    assert(hashtable_slot_heads[slot_idx]);
    hashtable_slot_heads[slot_idx] = hashtable_slot_heads[slot_idx]->next;
}

hashtable_entry_t *hashtable_query_entry(const char *key) {
    uint slot_idx = hashval(key);
    for (hashtable_entry_t *entry = hashtable_slot_heads[slot_idx]; entry; entry = entry->next)
        if (!strcmp(entry->key, key))
            return entry;
    return NULL;
}
