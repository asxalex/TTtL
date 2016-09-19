#
# Makefile
# alex, 2016-09-19 08:44
#
#
TARGET=tt
CC=gcc
CFLAGS=-I .
C_SOURCES=$(shell find . -name "*.c" ! -name "test_*.c")
C_OBJECTS=$(patsubst %.c, %.o, $(C_SOURCES))
HEADERS=$(shell find . -name "*.h")
TEMP=$(shell find . -name "*~")

$(TARGET): $(C_OBJECTS)
	$(CC) $^ -o $@

%.o: $(C_SOURCES) %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -r $(shell find . -name "*.o") $(TEMP) $(TARGET)

# vim:ft=make
#
