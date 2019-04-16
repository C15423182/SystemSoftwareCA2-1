#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8081

int main(int argc, char *argv[])
{
	int SID;
	struct sockaddr_in server;
	char clientMessage[1024];
	char serverMessage[1024];

	// create socket
	SID = socket(AF_INET, SOCK_STREAM, 0);
	if(SID == -1)
	{
		printf("Error creating socket!\n");
	}
	else
	{
		printf("Socket is live my pluggg\n");
	}


	// set sockaddr_in variables
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET; // IPV4 protocol

	if(connect(SID, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		printf("connect failed.Error\n");
		return 1;
	}
	
	printf("Successfully connected to server\n");


	while(1)
	{
		printf("\nEnter Message: \n");
		scanf("%s", clientMessage);

		// send data
		if(send(SID,clientMessage, strlen(clientMessage), 0) < 0)
		{
			printf("Error sending message\n");
			return 1;
		}

		// Receive a reply from the server
		if( recv(SID, serverMessage, 500, 0) < 0)
		{
			printf("Error receving data\n");
			break;
		}

		printf("\nServer sent: ");
		printf(serverMessage);
	}

	close(SID);
	return 0;
}