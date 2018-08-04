CFLAGS = -std=c99 -Werror -Wall -I ./src/

all: server client test

server: src/server.c
	gcc $(CFLAGS) -o bin/server src/server.c

client: src/client.c
	gcc $(CFLAGS) -o bin/client src/client.c

test: src/test/protocol.test.c
	gcc $(CFLAGS) -g -c -o obj/protocol.o src/protocol.c
	gcc $(CFLAGS) -g -c -o obj/utils.o src/utils.c
	gcc $(CFLAGS) -g -o bin/test/protocol.test obj/protocol.o obj/utils.o src/test/protocol.test.c

run-tests: bin/test/protocol.test
	./bin/test/protocol.test
