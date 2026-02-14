JULIA_DIR := $(shell julia -e 'print(joinpath(Sys.BINDIR, ".."))')

# Julia flags (tests only)
JULIA_INC  = -I$(JULIA_DIR)/include/julia
JULIA_LIBS = -L$(JULIA_DIR)/lib -Wl,-rpath,$(JULIA_DIR)/lib

CC = gcc

IDIR = .
TEST_DIR = tests

CFLAGS = -I$(IDIR)
LDFLAGS =
LDLIBS =

# Debug / sanitizer flags (for non-Julia builds)
CFLAGS_DEBUG = -g -O0 -Wall -Wextra -Wpedantic \
               -fsanitize=address,undefined -ffast-math
LDFLAGS_DEBUG = -fsanitize=address,undefined

OBJ = main.o matrix.o fmatrix.o

TARGET = main
TEST_TARGET = tests
TEST_TARGET_BINARY = run_tests

TEST_OBJ = \
    matrix.o \
    fmatrix.o \
    $(TEST_DIR)/main_tests.o \
    $(TEST_DIR)/tests.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

tests/%.o: tests/%.c
	$(CC) -c -o $@ $< $(CFLAGS) $(JULIA_INC) -I$(TEST_DIR)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(TEST_TARGET): $(TEST_OBJ)
	$(CC) -o $(TEST_TARGET_BINARY) $^ $(JULIA_LIBS) -ljulia -lcmocka

# Debug build for main only (sanitized, no Julia)
debug: CFLAGS += $(CFLAGS_DEBUG)
debug: LDFLAGS += $(LDFLAGS_DEBUG)
debug: clean $(TARGET)

.PHONY: clean debug tests

clean:
	rm -f *.o
	rm -f $(TEST_DIR)/*.o
	rm -f $(TARGET) $(TEST_TARGET_BINARY)

