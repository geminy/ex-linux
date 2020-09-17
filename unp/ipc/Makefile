.PHONY: UNP_DEFAULT
UNP_DEFAULT:

slot: slot.o
	gcc -o $@ $^

ftok: ftok.o
	gcc -o $@ $^

test: test.o libunp.so
	gcc -o $@ $< -L. -lunp

%.o: %.c
	gcc -c $<

libunp.so: px_ipc_name.c
	gcc -fpic -shared -o $@ $^

clean:
	rm -f *.so
	rm -f *.o
	rm -f test
	rm -f ftok
	rm -f slot
