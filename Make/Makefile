CC=gcc
SRC=main.c http.c jobs_queue.c
TEST_SRC=http.c test.c jobs_queue.c
CFLAGS=-Wall -O3 -I. 
EXEC=httpd
EXEC_TEST=test
OBJECTS=$(SRC:.c=.o)
all:$(EXEC)
$(EXEC):$(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC)
test:
	$(CC) $(CFLAGS) $(TEST_SRC) -g -o $(EXEC_TEST) 
.PHONY: clean
clean:
	rm -rf $(EXEC) $(EXEC_TEST) $(OBJECTS)
