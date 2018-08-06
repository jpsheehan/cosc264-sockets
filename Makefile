CFLAGS = -std=c99 -Werror -Wall -I ./src/

all: libs server client

libs:
	gcc $(CFLAGS) -c -o obj/protocol.o src/protocol.c
	gcc $(CFLAGS) -c -o obj/utils.o src/utils.c

server: libs src/server.c
	gcc $(CFLAGS) -o bin/server obj/protocol.o obj/utils.o src/server.c

client: libs src/client.c
	gcc $(CFLAGS) -o bin/client obj/protocol.o obj/utils.o src/client.c

test: libs src/test/protocol.test.c
	gcc $(CFLAGS) -o bin/test/protocol.test obj/protocol.o obj/utils.o src/test/protocol.test.c

run-tests: bin/test/protocol.test
	./bin/test/protocol.test

docs: doxygen.conf
	doxygen doxygen.conf
	make -C doc/latex

clean:
	rm -v obj/protocol.o obj/utils.o
	rm -Rv doc/*
	rm -v bin/server
	rm -v bin/client
	rm -v bin/test/*
