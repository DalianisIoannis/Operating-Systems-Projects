comp:
	@echo " Compile coord ...";
	gcc coordinator.c shared_memory.c sems_shm.c -o runner