

OBJS = strbuf.o

all:	test

test:	$(OBJS) main.o
	$(CC) -o $@ $^

clean:
	rm -f test $(OBJS) main.o
