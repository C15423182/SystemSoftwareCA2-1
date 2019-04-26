#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/socket.h>    
#include<arpa/inet.h> 
#include <sys/types.h>
#include <netinet/in.h>
#include <grp.h>
#include <pwd.h>

#define PORT 8082
void getUserRelatedID();
void getGroupIDs();
void sendFile(int sockfd);
int main(int argc, char *argv[])
{
    int sockfd;
    char buffer[1000];
    char server_reply[2000];
    char FOLDER[2000];
    char FILEPATH[2000];
    char USER[2000];
    char finalDetails[2000];
    ssize_t n;
  
    struct sockaddr_in servaddr; 
    /*
    if(argc !=2)
    {
        printf("Need to specify arguments\n");
        exit(1);
    }
    */
    //else
    //{
        //strcpy(FILEPATH, argv[1]); // copy 2nd argument into file path
        //strcpy(FOLDER, argv[1]); // copy 3rd argument into folder

        char *user = getenv("USER");
        //printf("Details provided: Username logged in: %s\nFIlEPATH: %s\nFOLDER: %s",user,FILEPATH,FOLDER);
        strcpy(USER, user);

        // add spaces between each name
        //strcat(FILEPATH, " ");
        //strcat(FOLDER, " ");
        //strcat(USER, " ");

        //strcat(FILEPATH,FOLDER); // concatentate filepath with folder, 
        //strcat(FILEPATH,USER); // concatentate filepath with user (so file path variable now contains, filepath, folder, and user)
        //copy the final string into one final string 
        //strcpy(finalDetails,FILEPATH);
        //printf("%s\n",finalDetails);


        //printf("Username: %s",USER);
        //printf("FOLDER: %s",FOLDER);
    //}
    

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


        /*
        if (send(sockfd,FILEPATH,strlen(FILEPATH),0) < 0)
        {
            printf("Error Sending message \n");
            return 1;
        }*/

        // send file first
        //sendFile(sockfd);

        // ask for folder
        printf("Enter folder name you want to send file to\n");
        scanf("%s",FOLDER);

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

        printf("Exiting client now, i put this in\n");
        /*
        if(recv(sockfd,server_reply,2000,0 ) < 0)
        {
            printf("Error Receving message");
            
        }
        else
        {
            printf("Server Reply: %s \n",server_reply );
        }
        */
        
        


    /*
    while (1)
    {
        // *********** Add main menu here, send file to marketing/sales/managemnet/etc , GET RID OF THE CHAT!!***********
        printf("Enter Message\n");
        scanf("%s",buffer);

        // if exit is typed, close connection and exit the client 
        if(strcmp(buffer, "exit") == 0)
		{
			close(sockfd);
			printf("[-]Disconnected from server.\n");
			exit(1);
		}

        
        if (send(sockfd,buffer,strlen(buffer),0) < 0)
        {
            printf("Error Sending message \n");
            return 1;
        }
        
        
        if(recv(sockfd,server_reply,2000,0 ) < 0)
        {
            printf("Error Receving message");
            break;
        }
        printf("Server Reply: %s \n",server_reply );
        
    }*/
    close(sockfd);
    return 0;
} // end main


void sendFile(int sockfd)
{
    // Client *********************************
    /*send to server*/
        
    char file_buffer[512]; 
    //char* file_name = FILEPATH;
    char temp[1000];
    printf("Enter filepath\n");
    scanf("%s",temp);
    FILE *file_open = fopen(temp, "r");
    if(file_open == NULL)
    {
        printf("File %s Cannot be found on client. exiting program..\n", temp);
        exit(EXIT_FAILURE);
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
            
        }
        printf("File transfer complete from client side\n");
        fclose(file_open);
    }
    
    
}