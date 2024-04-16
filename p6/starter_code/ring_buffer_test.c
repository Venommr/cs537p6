#include "ring_buffer.h"
#include <stdio.h>

int main()
{
    struct ring r;
    if (init_ring(&r) < 0)
    {
        printf("Failed to initialize ring buffer\n");
        return 1;
    }

    struct buffer_descriptor bd1 = {PUT, 1, 10, 0, 0};
    struct buffer_descriptor bd2 = {GET, 2, 0, 0, 0};

    ring_submit(&r, &bd1);
    ring_submit(&r, &bd2);

    struct buffer_descriptor bd_out1, bd_out2;
    ring_get(&r, &bd_out1);
    ring_get(&r, &bd_out2);

    printf("Retrieved items:\n");
    printf("Item 1: req_type=%d, k=%u, v=%u\n", bd_out1.req_type, bd_out1.k, bd_out1.v);
    printf("Item 2: req_type=%d, k=%u\n", bd_out2.req_type, bd_out2.k);

    return 0;
}