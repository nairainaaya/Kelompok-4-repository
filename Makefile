CC = gcc
CFLAGS = -Wall -Wextra -g

SRCS = main.c auth.c buku.c data.c input.c sorting.c transaksi.c
OBJS = $(SRCS:.c=.o)

ifeq ($(OS),Windows_NT)
    TARGET = perpustakaan.exe
    RM = cmd /C del /Q /F
else
    TARGET = perpustakaan
    RM = rm -f
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-$(RM) $(OBJS) $(TARGET)

.PHONY: all clean