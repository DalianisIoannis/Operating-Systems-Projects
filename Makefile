comp:
	@echo " Compile coord ...";
	gcc coordinator.c shared_memory.c sems_funcs.c -o runner -lm