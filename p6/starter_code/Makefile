.DEFAULT: all

CC = gcc
override CFLAGS += -c -g
override LDFLAGS += -lpthread
SERVER_OBJS = kv_store.o ring_buffer.o
CLIENT_OBJS = client.o ring_buffer.o
HEADERS = common.h

.PHONY: all, clean
all: client server

client: $(CLIENT_OBJS)
	$(CC) $(CLIENT_OBJS) $(LDFLAGS) -o $@

server: $(SERVER_OBJS)
	$(CC) $(SERVER_OBJS) $(LDFLAGS) -o $@

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<

clean: 
	rm -rf $(SERVER_OBJS) $(CLIENT_OBJS) server client

ring_buffer_test: ring_buffer_test.o ring_buffer.o
	$(CC) ring_buffer_test.o ring_buffer.o -o ring_buffer_test

ring_buffer_test.o: ring_buffer_test.c ring_buffer.h
	$(CC) $(CFLAGS) -c ring_buffer_test.c
