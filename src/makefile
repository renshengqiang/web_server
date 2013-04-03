all: web_server

web_server: main.o server.o utilities.o
	gcc -W -Wall main.o server.o utilities.o -o web_server

main.o: main.c server.h
	gcc -W -Wall -g -c main.c

server.o: server.c server.h utilities.h
	gcc -W -Wall -g -c server.c

utilities.o: utilities.c utilities.h
	gcc -W -Wall -g -c utilities.c

clean:
	rm -rf *.o web_server

