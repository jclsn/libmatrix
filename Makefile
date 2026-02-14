JULIA_DIR := $(shell julia -e 'print(joinpath(Sys.BINDIR, ".."))')

# Julia flags (tests only)
JULIA_INC  = -I$(JULIA_DIR)/include/julia
JULIA_LIBS = -L$(JULIA_DIR)/lib -Wl,-rpath,$(JULIA_DIR)/lib
JULIA_LINK = -ljulia

CMOCKA_LIBS = -lcmocka

TARGET = main
TEST_TARGET = run_tests

CC = gcc

# Include paths
IDIR = .
TEST_DIR = tests

# Base flags
CFLAGS = -I$(IDIR)
LDFLAGS =
LDLIBS =

# Debug / sanitizer flags (for non-Julia builds)
CFLAGS_DEBUG = -g -O0 -Wall -Wextra -Wpedantic \
               -fsanitize=address,undefined -ffast-math
LDFLAGS_DEBUG = -fsanitize=address,undefined

# Production objects
OBJ = main.o matrix.o fmatrix.o

# Test objects
TEST_OBJ = \
    matrix.o \
    fmatrix.o \
    $(TEST_DIR)/main_tests.o \
    $(TEST_DIR)/tests.o

# Compile rule (works for root and subdir)
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

# Production binary
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

# Test binary (Julia only here, NO sanitizers)
$(TEST_TARGET): CFLAGS += $(JULIA_INC) -I$(TEST_DIR)
$(TEST_TARGET): $(TEST_OBJ)
	$(CC) -o $@ $^ $(JULIA_LIBS) $(JULIA_LINK) $(CMOCKA_LIBS)

# Debug build for main only (sanitized, no Julia)
debug: CFLAGS += $(CFLAGS_DEBUG)
debug: LDFLAGS += $(LDFLAGS_DEBUG)
debug: clean $(TARGET)

# Normal test build (no sanitizers, Julia enabled)
tests: $(TEST_TARGET)

.PHONY: clean debug tests

clean:
	rm -f *.o
	rm -f $(TEST_DIR)/*.o
	rm -f $(TARGET) $(TEST_TARGET)

