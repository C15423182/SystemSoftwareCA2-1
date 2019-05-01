#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>    
#include <arpa/inet.h> 
#include <sys/types.h>
#include <netinet/in.h>
#include <grp.h>
#include <pwd.h>
#include <string.h>

#define PORT 8082
void getUserRelatedID();
void getGroupIDs();
void sendFile(int sockfd);
int main(int argc, char *argv[])
{
    int sockfd;
    char buffer[1000];
    char FOLDER[2000];
    char USER[2000];
    ssize_t n;
  
    struct sockaddr_in servaddr; 
    
    // get logged in user, store it in variable USER
    char *user = getenv("USER");
    strcpy(USER, user);

    
    

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd == -1)
    {
        printf("[-]Error in creating socket.\n");
		exit(1);
    }
	printf("[+]Client Socket is created.\n");


    
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
   
    if((connect(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr)) < 0))
    {
        printf("[-]Error in connection.\n");
		exit(1);
    }


    while(1)
    {
        // ask for folder
        printf("Enter folder name you want to send file to\n");
        printf("Sales\nMarketing\nOffers\nPromotions\nRoot\n");
        scanf("%s",FOLDER);
        if(strcmp(FOLDER,"Sales") !=0 && strcmp(FOLDER,"Marketing") !=0
            && strcmp(FOLDER,"Offers") !=0 && strcmp(FOLDER,"nPromotions") !=0
            && strcmp(FOLDER,"Root") !=0)
        {
            printf("Please enter valid destination\n");
        }
        else
        {
            printf("Correct folder has been captured\n");
            break;
        }
    }

    strcat(FOLDER, " ");
    strcat(USER, " ");
        
    //join two strings together to send
    strcat(FOLDER,USER);

    printf("Sending Credentials over\n");
    if (send(sockfd,FOLDER,strlen(FOLDER),0) < 0)
    {
        printf("Error Sending message \n");
        return 1;
    }

    // send file now
    sendFile(sockfd);

    //printf("Exiting client now..\n");
    //close(sockfd);
    //return 0;
    
    ///home/fayezrahman/Desktop/clientFiles/fayez.txt
    char server_reply[2000];
    if(recv(sockfd,server_reply,2000,0 ) < 0)
    {
        printf("Error Receving message");
        //break;
    }
    printf("Server Reply: %s \n",server_reply );

    /*
    while (1)
    {        
        char server_reply[2000];
        if(recv(sockfd,server_reply,2000,0 ) < 0)
        {
            printf("Error Receving message");
            break;
        }
        printf("Server Reply: %s \n",server_reply );
        
    }*/
} // end main


void sendFile(int sockfd)
{
    // Client *********************************
    /*send to server*/
        
    char file_buffer[512]; 
    FILE *file_open;
    char temp[1000];
    while(file_open == NULL)
    {
        printf("Enter filepath\n");
        scanf("%s",temp);
        file_open = fopen(temp, "r");
        if(file_open == NULL)
        {
            printf("File %s Cannot be found on client. Enter new file..\n", temp);
            
        }
        else
        {
            printf("[Client] Sending %s to the Server... ", temp);
            bzero(file_buffer, 512); 
            int block_size,i=0; 
            while((block_size = fread(file_buffer, sizeof(char), 512, file_open)) > 0) 
            {
                printf("Data Sent %d = %d\n",i,block_size);
                if(send(sockfd, file_buffer, block_size, 0) < 0) 
                {
                    printf("Sending failed\n");
                    exit(1);
                }
                bzero(file_buffer, 512);
                i++;
                
            }// end while
            printf("File transfer complete from client side\n");
            fclose(file_open);
        }// end else
    }// end big while
    
} // end main