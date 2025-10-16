producer:
	gcc -o producer posix_shm_consumer.c
consumer:
	gcc -o consumer posix_shm_producer.c

all: producer consumer
clean:
	rm -f producer consumer