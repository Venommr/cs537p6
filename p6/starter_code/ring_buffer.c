#include "ring_buffer.h"

#include "ring_buffer.h"
#include <stdio.h>

// Initialize the ring buffer
// Set p_tail, p_head, c_tail, and c_head to 0
// Return 0 on success, negative value on failure
int init_ring(struct ring *r)
{
    printf("init_ring: r = %p\n", (void *)r);
    if (r == NULL)
    {
        printf("init_ring: r is NULL\n");
        return -1;
    }
    r->p_tail = 0;
    r->p_head = 0;
    r->c_tail = 0;
    r->c_head = 0;
    pthread_mutex_init(&r->mutex, NULL); // Initialize the mutex
    pthread_cond_init(&r->cond, NULL);   // Initialize the condition variable
    printf("init_ring: Initialization successful\n");
    return 0;
}

// Submit a new item to the ring buffer
// Block if there's not enough space
// Make sure it's thread-safe
void ring_submit(struct ring *r, struct buffer_descriptor *bd)
{
    if (r == NULL || bd == NULL)
    {
        return;
    }
    pthread_mutex_lock(&r->mutex); // Lock the mutex to ensure exclusive access
    while ((r->p_head + 1) % RING_SIZE == r->c_tail)
    {
        // Buffer is full, wait on the condition variable
        pthread_cond_wait(&r->cond, &r->mutex);
    }
    r->buffer[r->p_head] = *bd;              // Add the item to the buffer
    r->p_head = (r->p_head + 1) % RING_SIZE; // Update the producer head
    pthread_cond_signal(&r->cond);           // Signal the condition variable to notify consumers
    pthread_mutex_unlock(&r->mutex);         // Unlock the mutex
}

// Get an item from the ring buffer
// Block if the buffer is empty
// Make sure it's thread-safe
void ring_get(struct ring *r, struct buffer_descriptor *bd)
{
    if (r == NULL || bd == NULL)
    {
        return;
    }
    pthread_mutex_lock(&r->mutex); // Lock the mutex to ensure exclusive access
    while (r->c_head == r->p_tail)
    {
        // Buffer is empty, wait on the condition variable
        pthread_cond_wait(&r->cond, &r->mutex);
    }
    *bd = r->buffer[r->c_head];              // Retrieve an item from the buffer
    r->c_head = (r->c_head + 1) % RING_SIZE; // Update the consumer head
    pthread_cond_signal(&r->cond);           // Signal the condition variable to notify producers
    pthread_mutex_unlock(&r->mutex);         // Unlock the mutex
}