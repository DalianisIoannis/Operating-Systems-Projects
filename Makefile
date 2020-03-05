comp:
	@echo "Compile coord ...";
	gcc coordinator.c shared_memory.c sems_funcs.c read_write.c -o runner -g3 -Wall -pthread -lm


see_ShM:
	@echo "Shared memory attached is:";
	ipcs -m
	@echo "View details for a segment with: ipcs -m -i <shmid>";
	@echo "Delete a segment with: ipcrm shm <shmid>";