myprogServer :	server.o
	gcc -o myprogServer server.o

server.o :	server.c
	gcc -c server.c


clean : 
	rm myprogServer server.o