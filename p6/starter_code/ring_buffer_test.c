#include "ring_buffer.h"
#include <stdio.h>
#include <pthread.h>

// Constants to define the number of producers, consumers, and items
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2
#define NUM_ITEMS 4

// Global ring buffer instance
struct ring r;

// Producer thread function
void *producer_thread(void *arg)
{
    int tid = *(int *)arg; // Thread ID

    // Submit NUM_ITEMS items to the ring buffer
    for (int i = 0; i < NUM_ITEMS; i++)
    {
        struct buffer_descriptor bd = {PUT, tid * 100 + i, tid * 100 + i, 0, 0};
        ring_submit(&r, &bd);
        printf("Producer %d submitted item: %d\n", tid, bd.v);
    }

    return NULL;
}

// Consumer thread function
void *consumer_thread(void *arg)
{
    int tid = *(int *)arg; // Thread ID

    // Retrieve NUM_ITEMS items from the ring buffer
    for (int i = 0; i < NUM_ITEMS; i++)
    {
        struct buffer_descriptor bd;
        ring_get(&r, &bd);
        printf("Consumer %d retrieved item: %d\n", tid, bd.v);
    }

    return NULL;
}

int main()
{
    // Initialize the ring buffer
    if (init_ring(&r) < 0)
    {
        printf("Failed to initialize ring buffer\n");
        return 1;
    }

    // Create producer and consumer threads
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];
    int producer_ids[NUM_PRODUCERS];
    int consumer_ids[NUM_CONSUMERS];

    for (int i = 0; i < NUM_PRODUCERS; i++)
    {
        producer_ids[i] = i;
        pthread_create(&producers[i], NULL, producer_thread, &producer_ids[i]);
    }

    for (int i = 0; i < NUM_CONSUMERS; i++)
    {
        consumer_ids[i] = i;
        pthread_create(&consumers[i], NULL, consumer_thread, &consumer_ids[i]);
    }

    // Wait for all producer and consumer threads to complete
    for (int i = 0; i < NUM_PRODUCERS; i++)
    {
        pthread_join(producers[i], NULL);
    }

    for (int i = 0; i < NUM_CONSUMERS; i++)
    {
        pthread_join(consumers[i], NULL);
    }

    return 0;
}