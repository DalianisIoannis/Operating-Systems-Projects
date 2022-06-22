CC = gcc
CFLAGS = -g3 -Wall
LDFLAGS = -lm

BDIR = bin
ODIR = build
IDIR = headers
SDIR = src

EXECUTABLE = runner

_DEPS = functions.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o Statistics.o WS.o LRU.o virtual_memory.o Address.o IPT.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(ODIR)/*.o
	# rm -f $(BDIR)/$(EXECUTABLE)