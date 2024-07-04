#include "hashmap.h"

size_t stringHashFunction(void* key) {
    unsigned long hash = 5381;
    char* str = (char*)key;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash;
}

HashMap* createHashMap(size_t size, size_t (*hashFunction)(void*), int (*compareFunction)(void*, void*)) {
    HashMap* table = malloc(sizeof(HashMap));
    table->size = size;
    table->entries = calloc(size, sizeof(HashMapEntry*));
    table->hashFunction = hashFunction;
    table->compareFunction = compareFunction;
    return table;
}

HashMapEntry* createEntry(void* key, void* value) {
    HashMapEntry* entry = malloc(sizeof(HashMapEntry));
    entry->key = key;
    entry->value = value;
    entry->next = NULL;
    return entry;
}

void insert(HashMap* table, void* key, void* value) {
    size_t index = table->hashFunction(key) % table->size;
    HashMapEntry* entry = table->entries[index];
    
    if (entry == NULL) {
        // No collision
        table->entries[index] = createEntry(key, value);
        return;
    }
    
    // Collision handling via chaining
    HashMapEntry* prev;
    while (entry != NULL) {
        // Update value if key already exists
        if (table->compareFunction(entry->key, key) == 0) {
            entry->value = value;
            return;
        }
        prev = entry;
        entry = entry->next;
    }
    prev->next = createEntry(key, value);
}

void* search(HashMap* table, void* key) {
    size_t index = table->hashFunction(key) % table->size;
    HashMapEntry* entry = table->entries[index];
    
    while (entry != NULL) {
        if (table->compareFunction(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}

void deleteEntry(HashMap* table, void* key) {
    size_t index = table->hashFunction(key) % table->size;
    HashMapEntry* entry = table->entries[index];
    HashMapEntry* prev = NULL;

    while (entry != NULL) {
        if (table->compareFunction(entry->key, key) == 0) {
            if (prev == NULL) {
                // Remove first bucket
                table->entries[index] = entry->next;
            } else {
                prev->next = entry->next;
            }
            free(entry);
            return;
        }
        prev = entry;
        entry = entry->next;
    }
}

int stringCompareFunction(void* key1, void* key2) {
    return strcmp((char*)key1, (char*)key2);
}

void freeHashMap(HashMap* table) {
    for (size_t i = 0; i < table->size; i++) {
        HashMapEntry* entry = table->entries[i];
        while (entry != NULL) {
            HashMapEntry* temp = entry;
            entry = entry->next;
            free(temp);
        }
    }
    free(table->entries);
    free(table);
}
