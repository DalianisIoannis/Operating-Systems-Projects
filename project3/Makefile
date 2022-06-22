CC = gcc
CFLAGS = -g3 -Wall
LDFLAGS = -lm

BDIR = bin
ODIR = build
IDIR = headers
SDIR = src

EXECUTABLE = runner

_DEPS = functions.h ShMFuncs.h SemFuncs.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o shared_memory.o feeder.o sems_funcs.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(ODIR)/*.o
	rm -f $(BDIR)/$(EXECUTABLE)

see_ShM:
	@echo "Shared memory attached is:";
	ipcs -m
	@echo "View details for a segment with: ipcs -m -i <shmid>";
	@echo "Delete a segment with: ipcrm shm <shmid>";

execute:
	valgrind ./runner 50 5000