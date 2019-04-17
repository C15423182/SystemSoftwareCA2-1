#include<stdio.h>
#include<string.h>    
#include<stdlib.h>    
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h>    
#include<pthread.h> 

#define PORT 8082

struct sockaddr_in cliaddr, servaddr; // global variables initliased so i can use them inside the server thread handler to determine who left!
void *server_handler (void *fd_pointer);

int main()
{
    int listenfd, connfd, *new_sock;
    socklen_t clilen;
    
   listenfd = socket(AF_INET,SOCK_STREAM,0);
   
    if (listenfd == -1)
    {    
        printf("[-]Error in creating Socket.\n");
		exit(1);
    }
    printf("[+]Server Socket is created.\n");


    //bzero(&servaddr,sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
   
   if (bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
   {
	    printf("[-]Error in binding.\n");
		exit(1);
   }
   
    printf("[+]Bind to port %d\n", PORT);

	if(listen(listenfd, 10) == 0)
	{
		printf("[+]Listening....\n");
	}
	else
	{
		printf("[-]Error in Listening.\n");
	}
   
    printf("[+]Waiting for incomming connection....\n");
    clilen = sizeof(cliaddr);

    while ((connfd = accept(listenfd,(struct sockaddr *)&cliaddr,&clilen)))
    {
        printf("Connection accepted from %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
        
        // create new thread to handle each incomming connection
        pthread_t server_thread;
        new_sock = malloc(sizeof *new_sock); // allocate the right amount of bytes required for the socket (instead of a hard coded integer)
        *new_sock = connfd;
        pthread_create(&server_thread,NULL,server_handler,(void*) new_sock); // create the thread by calling the server handler function
    }
   
    if (connfd < 0)
	{
		perror("Accecpt Failed");
		return 1;
	}
    
	return 0;

}

void *server_handler (void *fd_pointer)
{
	printf("Hello From Thread Server Handler \n");
	int sock = *(int *)fd_pointer;
    int read_size, write_size;
    char *message;
	static char client_message[2000];
    message = " \nCustom Message from thread handler \n";

    while((read_size = recv(sock,client_message,2000,0)) > 0)
    {

        if(strcmp(client_message, "exit") == 0)
        {
            printf("Client %s:%d has exited \n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
            break;
        }
        printf("Message Size %d \n", read_size); // optional 
        printf("Message Sent %s \n", client_message);
        write(sock,client_message,strlen(client_message));
        memset(client_message ,'\0', 2000); // free the buffer 
    }
    if(read_size == 0)
    {
        printf("Client %s:%d Disconnected \n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
        fflush(stdout);
    }
	else if(read_size == -1)
    {
        perror("recv failed");
    }
    free(fd_pointer);
    close(sock);
    pthread_exit(NULL); 
     
    return 0;
}