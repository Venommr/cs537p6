#include "ring_buffer.h"
#include <stdio.h>
#include <semaphore.h>

// Initialize the ring buffer
// Set p_tail, p_head, c_tail, and c_head to 0
// Initialize the semaphores buffer_slots and items
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
    sem_init(&r->buffer_slots, 0, RING_SIZE); // Initialize buffer_slots semaphore with RING_SIZE resources
    sem_init(&r->items, 0, 0);                // Initialize items semaphore with 0 resources
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

    printf("ring_submit: Waiting for available slot\n");
    sem_wait(&r->buffer_slots); // Wait for an available slot in the ring buffer
    printf("ring_submit: Got available slot\n");

    r->buffer[r->p_head] = *bd;              // Copy the buffer descriptor to the ring buffer
    r->p_head = (r->p_head + 1) % RING_SIZE; // Update the producer head

    printf("ring_submit: Posting to items semaphore\n");
    sem_post(&r->items); // Signal that a new item has been added to the ring buffer
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

    printf("ring_get: Waiting for available item\n");
    sem_wait(&r->items); // Wait for an available item in the ring buffer
    printf("ring_get: Got available item\n");

    *bd = r->buffer[r->c_head];              // Copy the buffer descriptor from the ring buffer
    r->c_head = (r->c_head + 1) % RING_SIZE; // Update the consumer head

    printf("ring_get: Posting to buffer_slots semaphore\n");
    sem_post(&r->buffer_slots); // Signal that a slot has become available in the ring buffer
}