task_worker:
	gcc task_worker.c -o task_worker -lrt
task_dispatcher:
	gcc task_dispatcher.c -o task_dispatcher -lrt
all: task_worker task_dispatcher
clean:
	rm -f task_worker task_dispatcher