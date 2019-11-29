comp:
	@echo " Compile coord ...";
	gcc coordinator.c sems_shm.c -o runner