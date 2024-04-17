#include "ring_buffer.h"
#include "common.h"

#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>
#include <stdio.h>


int init_ring(struct ring* r){
	if(r == NULL){
		return -1;
	}
	return 0;
}

void ring_submit(struct ring* r, struct buffer_descriptor* bd){
	index_t p_head, p_next;
	do {
		p_head = r->p_head;
		p_next = (p_head+1) % RING_SIZE;
		if(p_next == r->c_tail){
			sched_yield();
		} 
	} while(!atomic_compare_exchange_strong(&r->p_head, &p_head, p_next));
	memcpy(&r->buffer[p_head], bd, sizeof(struct buffer_descriptor));
	/*r->buffer[p_head].k = bd->k;
	r->buffer[p_head].v = bd->v;
	r->buffer[p_head].req_type = bd->req_type;
	r->buffer[p_head].res_off = bd->res_off;
	r->buffer[p_head].ready = 0;*/
	
	index_t p_tail;
	do{
		p_tail = r->p_tail;
	} while(!atomic_compare_exchange_strong(&r->p_tail, &p_tail, p_next));
}

void ring_get(struct ring* r, struct buffer_descriptor* bd){
	index_t c_head, c_next;
	do {
		c_head = r->c_head;
		c_next = (c_head + 1) % RING_SIZE;
		if(c_next == r->p_tail){
			sched_yield();
		}
	} while(!atomic_compare_exchange_strong(&r->c_head, &c_head, c_next));
		memcpy(bd, &r->buffer[c_head], sizeof(struct buffer_descriptor));
	/*bd->k = r->buffer[c_head].k;
	bd->v = r->buffer[c_head].v;
	bd->res_off = r->buffer[c_head].res_off;
	bd->req_type = r->buffer[c_head].req_type;
*/
	index_t c_tail;
	do{
		c_tail = r->c_tail;
	}while (!atomic_compare_exchange_strong(&r->c_tail, &c_tail, c_next));
	
}

