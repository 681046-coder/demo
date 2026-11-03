CC      ?= gcc
CFLAGS  ?= -std=c99 -Wall -Wextra -O2
TARGET  := pc_stock

all: $(TARGET)

$(TARGET): src/pc_stock.c
	$(CC) $(CFLAGS) -o $@ $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe

.PHONY: all run clean
