
# Project #2 (CS3361) - Cooke Syntax Parser

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra
TARGET = cooke_parser

all: $(TARGET)

$(TARGET): front.c parser.c front.h parser.h
	$(CC) $(CFLAGS) -o $(TARGET) front.c parser.c

clean:
	rm -f $(TARGET) $(TARGET).exe

.PHONY: all clean
