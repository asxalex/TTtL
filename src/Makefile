#
# Makefile
# alex, 2016-09-19 08:44
#
#
TARGET=tt
CC=gcc
CFLAGS=-g -I .
C_SOURCES=$(shell find . -name "*.c" ! -name "test_*.c")
C_OBJECTS=$(patsubst %.c, %.o, $(C_SOURCES))
HEADERS=$(shell find . -name "*.h")

$(TARGET): $(C_OBJECTS)
	$(CC) $^ -g -o $@

%.o: $(C_SOURCES) %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -r $(shell find . -name "*.o") $(TARGET)

# vim:ft=make
#
