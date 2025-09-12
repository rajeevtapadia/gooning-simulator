CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
LDFLAGS = -lraylib -lm
TARGET = main.out

SRC := $(shell find . -name '*.c')
OBJ := $(patsubst %.c,build/%.o,$(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(TARGET) build
