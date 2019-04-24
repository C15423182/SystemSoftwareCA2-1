myprogServer :	threadedServer.o
	gcc -o myprogServer threadedServer.o -lpthread


myprogClient : threadedClient.o
	gcc -o myprogClient threadedClient.o

threadedClient.o : threadedClient.c
	gcc -c threadedClient.c

threadedServer.o :	threadedServer.c
	gcc -c threadedServer.c -lpthread

clean : 
	rm myprogServer myprogClient threadedServer.o threadedClient.o