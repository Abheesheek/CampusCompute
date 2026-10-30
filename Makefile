CC = gcc
CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -O2 -pthread -Iinclude
TARGET = campuscompute
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
all: $(TARGET)
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -pthread
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(OBJ) $(TARGET) campuscompute.log
test: $(TARGET)
	bash tests/smoke.sh
.PHONY: all clean test
