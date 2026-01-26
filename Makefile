TARGET = matrix
IDIR =./include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=./

LIBS=

_OBJ = main.o matrix.o fmatrix.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
	rm -f $(ODIR)/$(TARGET)

