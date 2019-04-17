myprogServer :	threadedServer.o
	gcc -o myprogServer threadedServer.o -lpthread

threadedServer.o :	threadedServer.c
	gcc -c threadedServer.c -lpthread

clean : 
	rm myprogServer threadedServer.o