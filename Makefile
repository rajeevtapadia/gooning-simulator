CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lraylib -lm
TARGET = main.out
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
