TARGET = matrix
IDIR = ./include
CC = gcc

CFLAGS = -I$(IDIR)
CFLAGS_DEBUG = -I$(IDIR) -g -O0 -Wall -Wextra -Wpedantic -fsanitize=address,undefined
LDFLAGS_DEBUG = -fsanitize=address,undefined

ODIR = ./

LIBS =

_OBJ = main.o matrix.o fmatrix.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

debug: CFLAGS=$(CFLAGS_DEBUG)
debug: LIBS=$(LDFLAGS_DEBUG)
debug: clean $(TARGET)

.PHONY: clean debug

clean:
	rm -f $(ODIR)/*.o
	rm -f $(ODIR)/$(TARGET)

