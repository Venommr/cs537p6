#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>


#include "ring_buffer.h"
#include "common.h"

struct ring* ring;
char* shmem_area;
size_t table_size;

typedef struct node{
	key_type k;
	value_type v;
	struct node* next;
} node;

typedef struct{
	node* head;
	pthread_mutex_t lock;
} bucket;

typedef struct{
	size_t size;
	bucket* buckets;
} hashtable;

hashtable* g_ht;

hashtable* init_hashtable(size_t size){
	hashtable* ht = (hashtable*) malloc(sizeof(hashtable));
	ht->size = size;
	ht->buckets = calloc(size, sizeof(bucket));
	for(int i = 0; i < size; i++){
		pthread_mutex_init(&ht->buckets[i].lock, NULL);
	}
	return ht;
}


void put(key_type k, value_type v){
	size_t bucket_idx = hash_function(k, g_ht->size);
	pthread_mutex_lock(&g_ht->buckets[bucket_idx].lock);
	printf("put(%d, %d)\n", k, v);
	node* cur = g_ht->buckets[bucket_idx].head;
	if(cur == NULL){
		cur = (node*)malloc(sizeof(node));
		cur->k = k;
		cur->v = v;
		cur->next = NULL;
	} else {
		while(cur->next != NULL){
			if(cur->k == k){
				cur->v = v;
				pthread_mutex_unlock(&g_ht->buckets[bucket_idx].lock);
				return;
			}
			cur = cur->next;
		}
		cur->next = (node*)malloc(sizeof(node));
		cur->k = k;
		cur->v = v;
		cur->next = NULL;
	}
	pthread_mutex_unlock(&g_ht->buckets[bucket_idx].lock);
}

value_type get(key_type k){
	size_t bucket_idx = hash_function(k, g_ht->size);
	pthread_mutex_lock(&g_ht->buckets[bucket_idx].lock);
	node* cur = g_ht->buckets[bucket_idx].head;
	while(cur != NULL){
		if(cur->k == k){
			value_type ret_v = cur->v;
			pthread_mutex_unlock(&g_ht->buckets[bucket_idx].lock);
			printf("get(%d) found %d\n", k, ret_v);
			return ret_v;
		}
		cur = cur->next;
	}
	printf("get(%d) found nothing\n", k);
	pthread_mutex_unlock(&g_ht->buckets[bucket_idx].lock);
	return 0;
}

void* server(void* arg){
	//printf("\n");
	
	while(1){
		struct buffer_descriptor bd;
		ring_get(ring, &bd);
		if(bd.req_type == GET){
			put(bd.k, bd.v);
		} else {
			value_type v = get(bd.k);
			struct buffer_descriptor* result = (struct buffer_descriptor*)(shmem_area + bd.res_off);	
			result->k = bd.k;
			result->v = v;
			result->ready = 1;
			//update request status board
		}
	}
}

int main(int argc, char** argv){
	int num_threads = 1;
    size_t hashtable_size = 1024;

    int opt;
    while ((opt = getopt(argc, argv, "n:s:")) != -1) {
        switch (opt) {
            case 'n':
                num_threads = atoi(optarg);
                break;
            case 's':
                hashtable_size = atoi(optarg);
                break;
            default:
                printf("Usage: ./server [-n num_threads] [-s hashtable_size]\n");
                exit(EXIT_FAILURE);	
        }
    }
	
	int fd = open("shmem_file", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (fd < 0)
		perror("open");

	struct stat st;
	fstat(fd, &st);
	shmem_area = mmap(NULL, st.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
	if (shmem_area == (void *)-1) 
		perror("mmap");

	ring = (struct ring*)shmem_area;
	/* mmap dups the fd, no longer needed */
	close(fd);

	g_ht = init_hashtable(hashtable_size);
	
	pthread_t threads[num_threads];
	for(int i = 0; i < num_threads; i++){
		pthread_create(&threads[i], NULL, server, NULL);
	}

	for(int i = 0; i < num_threads; i++){
		pthread_join(threads[i], NULL);
	}

	munmap(shmem_area, st.st_size);
}
