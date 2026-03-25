CC = gcc
CFLAGS = -Wall -Wextra -g

SRCS = main.c graphe.c LSTPRIM.c ELTARTICLE.c
OBJS = $(SRCS:.c=.o)
TARGET = reseau

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
