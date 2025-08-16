CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lraylib -lm
TARGET = main.out

SRC := main.c $(wildcard block-shapes/*.c)
OBJ := $(patsubst %.c,build/%.o,$(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Create directories as needed and compile
build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(TARGET) build
