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
void sendFile(char FILEPATH, int sockfd);
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
    if(argc !=3)
    {
        printf("Need to specify arguments\n");
        exit(1);
    }
    */
    //else
    //{
        strcpy(FILEPATH, argv[1]); // copy 2nd argument into file path
        strcpy(FOLDER, argv[2]); // copy 3rd argument into folder

        char *user = getenv("USER");
        //printf("Details provided: Username logged in: %s\nFIlEPATH: %s\nFOLDER: %s",user,FILEPATH,FOLDER);
        strcpy(USER, user);

        // add spaces between each name
        strcat(FILEPATH, " ");
        strcat(FOLDER, " ");
        strcat(USER, " ");

        //strcat(FILEPATH,FOLDER); // concatentate filepath with folder, 
        //strcat(FILEPATH,USER); // concatentate filepath with user (so file path variable now contains, filepath, folder, and user)
        //copy the final string into one final string 
        //strcpy(finalDetails,FILEPATH);
        //printf("%s\n",finalDetails);


        printf("Username: %s",USER);
        printf("FOLDER: %s",FOLDER);
        printf("FILEPATH: %s",FILEPATH);
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

        sendFile(FILEPATH,sockfd);


        
        /*
        printf("Sending folder destination\n");
        if (send(sockfd,FOLDER,strlen(FOLDER),0) < 0)
        {
            printf("Error Sending message \n");
            return 1;
        }


        printf("Sending username logged in\n");
        if (send(sockfd,USER,strlen(USER),0) < 0)
        {
            printf("Error Sending message \n");
            return 1;
        }
        */

        if(recv(sockfd,server_reply,2000,0 ) < 0)
        {
            printf("Error Receving message");
            
        }
        else
        {
            printf("Server Reply: %s \n",server_reply );
        }
        


    
    while (1)
    {
        // *********** Add main menu here, send file to marketing/sales/managemnet/etc , GET RID OF THE CHAT!!***********
        printf("0- to transfer to root folder\nJust specify file path\n");
        scanf("%s",buffer);

        // if exit is typed, close connection and exit the client 
        if(strcmp(buffer, "exit") == 0)
		{
			close(sockfd);
			printf("[-]Disconnected from server.\n");
			exit(1);
		}

        
        // if exit is typed, close connection and exit the client 
        if(strcmp(buffer, "0") == 0)
		{
            char filePath[2000];
			printf("Type file path\n");
            scanf("%s",filePath);

            if (send(sockfd,filePath,strlen(filePath),0) < 0)
            {
                printf("Error Sending message \n");
                return 1;
            }
		}

        /*
        if (send(sockfd,buffer,strlen(buffer),0) < 0)
        {
            printf("Error Sending message \n");
            return 1;
        }
        */
        
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


void getUserRelatedID()
{
    // find all ID's relating to each client
    uid_t uid = getuid();
    uid_t gid = getgid();
    uid_t ueid = geteuid();
    uid_t geid = getegid();

    printf("User ID: %d\n",uid);
    printf("Group ID: %d\n",gid);
    printf("Effective User ID: %d\n",ueid);
    printf("Effective User ID: %d\n",geid);
}


// void getGroupIDs()
// {
//     int j, ngroups;
//     gid_t *groups;
//     struct passwd *pw;
//     struct group *gr;
//     int myGroupIds[50];

//     //char *user = getenv("USER");

//     ngroups = 50;

//     groups = malloc(ngroups * sizeof (gid_t));
//     if (groups == NULL) {
//         perror("malloc");
//         exit(EXIT_FAILURE);
//     }

//     /* Fetch passwd structure (contains first group ID for user) */

//     pw = getpwnam(user);
//     if (pw == NULL) {
//         perror("getpwnam");
//         exit(EXIT_SUCCESS);
//     }

//     /* Retrieve group list */

//     if (getgrouplist(user, pw->pw_gid, groups, &ngroups) == -1) {
//         fprintf(stderr, "getgrouplist() returned -1; ngroups = %d\n",
//                 ngroups);
//         exit(EXIT_FAILURE);
//     }

//     /* Display list of retrieved groups, along with group names */

//     fprintf(stderr, "User : %s belongs in %d groups\n", user, ngroups);
//     for (j = 0; j < ngroups; j++) 
//     {
//         printf("%d", groups[j]);
//         myGroupIds[j] = groups[j]; // add each group ID to array so we can search for them later.
//         gr = getgrgid(groups[j]);
//         if (gr != NULL)
//             printf(" (%s)", gr->gr_name);
//         printf("\n");
//     }


// // ************************************ PUT THIS IN A SEPERATE FUNCTION LATER *********************************
//     // search for group ID to check if user can access or not
//     int found = 0;
//     int searchfor = 1001;

//     for (int i=0; i<ngroups; i++)
//     {
//         if(myGroupIds[i] == searchfor)
//         {
//             found = 1;
//             break;
//         }
//     }

//     if(found == 1)
//     {
//         printf("\n group number found, access allowed\n");
//     }
//     else
//     {
//         printf("\n %s is not part of the group folder\n", user);
//     }
// }


void sendFile(char FILEPATH, int sockfd)
{
    // Client *********************************
    /*send to server*/
        
    char file_buffer[512]; 
    char* file_name = FILEPATH;
    printf("[Client] Sending %s to the Server... ", file_name);
    FILE *file_open = fopen(file_name, "r");
    bzero(file_buffer, 512); 
    int block_size,i=0; 
    while((block_size = fread(file_buffer, sizeof(char), 512, file_open)) > 0) 
    {
        printf("Data Sent %d = %d\n",i,block_size);
        if(send(sockfd, file_buffer, block_size, 0) < 0) 
        {
            exit(1);
        }
        bzero(file_buffer, 512);
        i++;
        
    }
    printf("File transfer complete from client side\n");
    fclose(file_open);
}