SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)

.PHONY: UNP_DEFAULT
UNP_DEFAULT: test server client

.PHONY: test
test:
	@echo $(SOURCES)
	@echo $(OBJECTS)

server: daytimetcpsrv.o
	gcc -o $@ $^

client: daytimetcpcli.o
	gcc -o $@ $^

%.o: %.c
	gcc -c $<

clean:
	rm -f *.o
	rm -f test
	rm -f server
	rm -f client
