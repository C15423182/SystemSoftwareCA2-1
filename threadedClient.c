#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/socket.h>    
#include<arpa/inet.h> 
#include <sys/types.h>
#include <netinet/in.h>

#define PORT 8082

int main()
{
    int sockfd;
    char buffer[1000];
    char server_reply[2000];
    ssize_t n;
  
   struct sockaddr_in servaddr; 

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd == -1)
    {
        printf("[-]Error in creating socket.\n");
		exit(1);
    }
	printf("[+]Client Socket is created.\n");


    //bzero(&servaddr,sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
   
    if((connect(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr)) < 0))
    {
        printf("[-]Error in connection.\n");
		exit(1);
    }
	 
    while (1)
    {
        printf("Enter a Message: ");
        scanf("%s",buffer);
        
        if (send(sockfd,buffer,strlen(buffer),0) < 0)
        {
            printf("Error Sending message \n");
            return 1;
        }
        // if exit is typed, close connection and exit the client 
        if(strcmp(buffer, "exit") == 0)
		{
			close(sockfd);
			printf("[-]Disconnected from server.\n");
			exit(1);
		}
        if(recv(sockfd,server_reply,2000,0 ) < 0)
        {
            printf("Error Receving message");
            break;
        }
        printf("Server Reply: %s \n",server_reply );
        
    }
        close(sockfd);
        return 0;
} // end main