all: soma_server

soma_server: soma_server.c opc/opc_server.c
	gcc -Wall -o $@ $^

run: soma_server
	./soma_server -f config.txt -l /dev/ttyUSB*

clean:
	rm -f soma_server

anyway: clean all
