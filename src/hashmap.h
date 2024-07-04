#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct HashMapEntry {
    void *key;
    void *value;
    struct HashMapEntry *next;
} HashMapEntry; 


typedef struct HashMap {
    HashMapEntry** entries;
    size_t size;
    size_t (*hashFunction)(void* key);
    int (*compareFunction)(void* key1, void* key2);
} HashMap;

size_t stringHashFunction(void* key);

HashMap* createHashMap(size_t size, size_t (*hashFunction)(void*), int (*compareFunction)(void*, void*));
HashMapEntry* createEntry(void* key, void* value);

void insert(HashMap* table, void* key, void* value);

void* search(HashMap* table, void* key);

void deleteEntry(HashMap* table, void* key);
int stringCompareFunction(void* key1, void* key2);

void freeHashTable(HashMap* table);

#endif // !HASH_MAP_H

