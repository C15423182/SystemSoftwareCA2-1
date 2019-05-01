myprogServer :	threadedServer.o copyFile.o
	gcc -o myprogServer threadedServer.o copyFile.o -lpthread

myprogClient : threadedClient.o
	gcc -o myprogClient threadedClient.o

threadedClient.o : threadedClient.c
	gcc -c threadedClient.c

threadedServer.o :	threadedServer.c
	gcc -c threadedServer.c -lpthread

copyFile.o:	copyFile.c functions.h
	gcc -c copyFile.c

clean : 
	rm myprogServer myprogClient threadedServer.o threadedClient.o copyFile.o