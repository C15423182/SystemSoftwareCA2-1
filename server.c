#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8081

int main(int argc, char *argv[])
{
	int socket_discriptor; 
	int clientSocket;
	int conntSize; // Size of struct 
	int READSIZE; // Size of sockaddr for client connection

	struct sockaddr_in server, client;
	char message[1024];

	// create socket
	socket_discriptor = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_discriptor == -1)
	{
		printf("Could not create socket");
	}
	else
	{
		printf("Socket sucessfully created!\n");
	}

	// set sockaddr_in variables
	server.sin_port = htons(PORT);// set the port for communication
	server.sin_family = AF_INET; // use IPV4 protocol
	server.sin_addr.s_addr = INADDR_ANY; 
	// when the INADDR_ANY is specified in the bind call, 
	// the socket will be bound to all local interfaces



	// bind time

	if(bind(socket_discriptor,(struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("Bind issue!!\n");
		return 1;
	}
	else
	{
		printf("Bind complete fam!\n");
	}

	// listen for a connection 
	listen(socket_discriptor, 3);

	// accept any incomming connection
	printf("Waiting for connection from client>>\n");
	conntSize = sizeof(struct sockaddr_in);



	clientSocket = accept(socket_discriptor, (struct sockaddr *)&client, (socklen_t*)&conntSize);
	if(clientSocket < 0)
	{
		perror("Can't establish connection");
		return 1;
	}
	else
	{
		printf("Connection accepted from %s:%d\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port) );
	}


	while(1)
	{
		if(strcmp(message, "exit") == 0)
		{
			printf("Dissconcted from %s:%d\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port));
			break;
		}
		memset(message, 0, 1024);
		READSIZE = recv(clientSocket, message, 2000, 0);
		printf("Client %s:%d said: %s",inet_ntoa(server.sin_addr),ntohs(server.sin_port),message);
		write(clientSocket,"What!?", strlen("What!?"));
	}

	if(READSIZE == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(READSIZE == -1)
    {
        perror("read error");
    }
     
    return 0;
	
}