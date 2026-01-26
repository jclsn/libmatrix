JULIA_DIR := $(shell julia -e 'print(joinpath(Sys.BINDIR, ".."))')

TARGET = matrix
IDIR = ./include
CC = gcc

CFLAGS = -I$(IDIR) -I$(JULIA_DIR)/include -ffast-math
CFLAGS_DEBUG = -I$(IDIR) -g -O0 -Wall -Wextra -Wpedantic -fsanitize=address,undefined -I$(JULIA_DIR)/include -ffast-math
LDFLAGS_DEBUG = -fsanitize=address,undefined

LDFLAGS := -L$(JULIA_DIR)/lib -Wl,-rpath,$(JULIA_DIR)/lib
LDLIBS  := -ljulia

ODIR = ./

LIBS =

_OBJ = main.o matrix.o fmatrix.o tests.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(LDFLAGS) $(LDLIBS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(LDFLAGS) $(LDLIBS)


debug: CFLAGS=$(CFLAGS_DEBUG)
debug: LIBS=$(LDFLAGS_DEBUG)
debug: clean $(TARGET)

.PHONY: clean debug

clean:
	rm -f $(ODIR)/*.o
	rm -f $(ODIR)/$(TARGET)

