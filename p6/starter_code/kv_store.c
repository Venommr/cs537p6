#include "common.h"
#include "ring_buffer.h"
#include <pthread.h>

// Define the hash table size
#define TABLE_SIZE 1000

// Entry structure for the hash table
typedef struct
{
    key_type key;
    value_type value;
    pthread_mutex_t lock; // Per-entry lock for fine-grained synchronization
} entry_t;

// Hash table structure
typedef struct
{
    entry_t entries[TABLE_SIZE];
} hashtable_t;

// Function to insert a key-value pair into the hash table
void put(hashtable_t *ht, key_type key, value_type value)
{
    index_t index = hash_function(key, TABLE_SIZE); // Compute the hash index

    pthread_mutex_lock(&ht->entries[index].lock); // Lock the entry
    ht->entries[index].key = key;
    ht->entries[index].value = value;
    pthread_mutex_unlock(&ht->entries[index].lock); // Unlock the entry
}

// Function to retrieve the value associated with a key from the hash table
value_type get(hashtable_t *ht, key_type key)
{
    index_t index = hash_function(key, TABLE_SIZE); // Compute the hash index

    pthread_mutex_lock(&ht->entries[index].lock); // Lock the entry
    value_type value = ht->entries[index].key == key ? ht->entries[index].value : 0;
    pthread_mutex_unlock(&ht->entries[index].lock); // Unlock the entry

    return value;
}

// Server thread function
void *server_thread(void *arg)
{
    // TODO: Implement the server thread logic
    // - Fetch requests from the Ring Buffer
    // - Process requests and update the Request-status Board
    // - Use the KV Store (hashtable) to handle PUT and GET requests
    // - Ensure proper synchronization when accessing shared data structures
}

int main(int argc, char *argv[])
{
    // TODO: Parse command-line arguments (-n and -s)
    // TODO: Initialize the Key-Value Store (hashtable)
    // TODO: Create server threads
    // TODO: Wait for server threads to complete
    return 0;
}