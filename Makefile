CC = gcc
CFLAGS = -Wall -Werror -g

TARGET = jiamingsh
SRCS = main.c lexer.c ast.c parser.c executor.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJS) $(TARGET).dSYM