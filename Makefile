all: server client test

server: src/server.c
	gcc -std=c99 -Werror -Wall -g -o bin/server src/server.c

client: src/client.c
	gcc -std=c99 -Werror -Wall -g -o bin/client src/client.c

test: src/test/protocol.test.c
	gcc -std=c99 -Werror -Wall -g -o bin/test/protocol.test src/test/protocol.test.c

run-test: bin/test/protocol.test
	./bin/protocol.test
