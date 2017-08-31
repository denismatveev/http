CC=gcc
SRC=main.c 
CFLAGS=-Wall -O3 -I. -static
EXEC=httpd
OBJECTS=$(SRC:.c=.o)
all:$(EXEC)
$(EXEC):$(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC)

.PHONY: clean
clean:
	rm -rf $(EXEC) $(OBJECTS)
