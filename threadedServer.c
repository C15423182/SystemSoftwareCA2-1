#include <stdio.h>
#include <string.h>    
#include <stdlib.h>    
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>    
#include <pthread.h> 
#include <grp.h>
#include <pwd.h>
#include <string.h>

//global variables
#define PORT 8082
int userID=0;
gid_t myGroupIds[50] = {}; // temporary array to hold user group ID's

// declare function
void getGroupIDs(char *user);
void changeID(int myID);
void getFile(int sock);
void copyFileToDestination();
void *server_handler (void *fd_pointer); // server handler function that will handle each thread coming in. 

pthread_mutex_t lock_x; // this is used for the mutex lock for shared resources
struct sockaddr_in cliaddr, servaddr; // global variables initliased so i can use them inside the server thread handler to determine who left!

char userToSearchFor[100];
char FolderToSearchFor[100];
int sock;


int main()
{
    int listenfd, connfd, *new_sock;
    socklen_t clilen;

    listenfd = socket(AF_INET,SOCK_STREAM,0);
    
    // create the lock 
    pthread_mutex_init(&lock_x,NULL);


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
    printf("Hello from the thread function my ID is : %d\n",pthread_self());

	sock = *(int *)fd_pointer;
    int read_size, write_size;
	static char client_message[2000];

    while(1)
    {
        
        if(strcmp(client_message, "exit") == 0)
        {
            printf("Client %s:%d has exited \n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
            break;
        }

    
        //receive credentials
        read_size = recv(sock,client_message,2000,0);

        if(read_size == 0 )
        {
            //printf("Error receeiving credetnials\n");
            memset(client_message ,'\0', 2000); // free the buffer 
            fflush(stdout);
            break;
        }
        else
        {
            printf("Message Sent From client is: %s \n", client_message);
            /* This for loop here splits the strings received from the client based on WHITE SPACE characters*/
            
            char newString[100][100]; 
            int i,j,ctr;
            j=0; ctr=0;

            for(i=0;i<=(strlen(client_message));i++)
            {
                // if space found, assign NULL into newString[ctr]
                if(client_message[i]==' ')
                {
                    newString[ctr][j]='\0';
                    ctr++;  //for next word
                    j=0;    //for next word, init index to 0
                }
                else
                {
                    newString[ctr][j]=client_message[i];
                    j++;
                }
            }
            printf("Credentials Extracted are:\n");
            

            // after we splitted the message copy it to each string
            strcpy(FolderToSearchFor,newString[0]);
            strcpy(userToSearchFor,newString[1]);
            
            // check copy has worked, printf credentials
            printf("Folder: %s\n",FolderToSearchFor);
            printf("Username: %s\n",userToSearchFor);
            
            memset(client_message ,'\0', 2000); // free the buffer 
            

            // Then receive file
            char file_buffer[512]; // Receiver buffer
            char* file_name = "/tmp/test.txt";
            FILE *file_open = fopen(file_name, "w");
            if(file_open == NULL)
            {
                printf("File %s Cannot be opened file on server.\n", file_name);
            }
            else                
            {
                // code is getting here but not executing??
                bzero(file_buffer, 512); 
                int block_size = 0;
                int i=0;
                while((block_size = recv(sock, file_buffer, 512, 0)) > 0) 
                {
                    printf("Data Received %d = %d\n",i,block_size);
                    int write_sz = fwrite(file_buffer, sizeof(char), block_size, file_open);
                    bzero(file_buffer, 512);
                    i++;
                }
                    
            }
            printf("Transfer Complete to tmp folder!\n");
            fclose(file_open); 
            //*****************************


            // call get Group ID's
            getGroupIDs(userToSearchFor);

        
        } // end else


        // printf("Message Sent %s \n", client_message);
        // write(sock,client_message,strlen(client_message));

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


void getGroupIDs(char *user)
{
    int j, ngroups;
    gid_t *groups;
    struct passwd *pw;
    struct group *gr;
    ngroups = 50;

    groups = malloc(ngroups * sizeof (gid_t));
    if (groups == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /* Fetch passwd structure (contains first group ID for user) */

    pw = getpwnam(user);
     if (pw == NULL) {
        perror("getpwnam");
        exit(EXIT_SUCCESS);
    }

    /* Retrieve group list */

    if (getgrouplist(user, pw->pw_gid, groups, &ngroups) == -1) {
        fprintf(stderr, "getgrouplist() returned -1; ngroups = %d\n",
                ngroups);
         exit(EXIT_FAILURE);
    }

    /* Display list of retrieved groups, along with group names */

    //fprintf(stderr, "User : %s belongs in %d groups\n", user, ngroups);
     for (j = 0; j < ngroups; j++) 
     {
         //printf("%d", groups[j]);
        myGroupIds[j] = groups[j]; // add each group ID to array so we can search for them later.
        gr = getgrgid(groups[j]);
        if (gr != NULL)
            //printf(" (%s)", gr->gr_name);
        printf("\n");
    }

    // from the retrieved groups assign the ID of the user that is actually logged in on the client
    userID = myGroupIds[0];
    
    printf("User %s has the following ID: %d\n",userToSearchFor, userID);
    // call changeID 
    changeID(userID);
 
}

void changeID(int myID)
{
    
    uid_t uid = getuid();
    uid_t gid = getgid();
    uid_t ueid = geteuid();
    uid_t geid = getegid();

    printf("User ID: %d\n",uid);
    printf("Effective User ID: %d\n",ueid);

    if (chown("/tmp/test.txt", userID, userID) == -1)
    {
        printf("Chown failed\n");
    }


    // get access to the lock
    pthread_mutex_lock(&lock_x);

    // attmepting to change ID'S 
    printf("Changing ID's to user logged in\n");

    setgroups(50, myGroupIds); // set 50 as size of the list
    if(setreuid(myID, uid) < 0)
    {
        printf("Error in changing REUID\n");
    }


    if(seteuid(myID) < 0)
    {
        printf("Error in changing EUID\n");
    }


    printf("ID after it got changed\n");

    printf("User ID: %d\n",getuid());
    printf("Effective User ID: %d\n",geteuid());

    // copy files to destination 
    copyFileToDestination();

    // release the lock 
    pthread_mutex_unlock(&lock_x);

    // set back to root
    int rootID = 0;
    setuid(rootID);
    setreuid((uid_t)rootID, uid);
    setregid((uid_t)rootID, gid);
    seteuid(rootID);
    setegid(rootID);
}


void getFile(int sock)
{
    char file_buffer[512]; // Receiver buffer
    char* file_name = "/tmp/test.txt";
    FILE *file_open = fopen(file_name, "w");
    if(file_open == NULL)
    {
        printf("File %s Cannot be opened file on server.\n", file_name);
    }
    else 
    {
        bzero(file_buffer, 512); 
        int block_size = 0;
        int i=0;
        while((block_size = recv(sock, file_buffer, 512, 0)) > 0) 
        {
            printf("Data Received %d = %d\n",i,block_size);
            int write_sz = fwrite(file_buffer, sizeof(char), block_size, file_open);
            bzero(file_buffer, 512);
            i++;
        }
        
    }
    printf("Transfer Complete!\n");
    fclose(file_open); 
    //*****************************
}

void copyFileToDestination()
{
    // If file entered is Sales
    if(strcmp(FolderToSearchFor,"Sales") == 0)
    {
        char command[500];
        int checker;
        strcpy(command, "cp /tmp/test.txt /home/fayezrahman/Desktop/serverFiles/Sales");
        system(command);
        if(checker == -1)
        {
            printf("System command failed\n");
        }

        if( access( "/home/fayezrahman/Desktop/serverFiles/Sales/test.txt", F_OK ) != -1 ) 
        {
            char replyMessage[100] = "Transfer Sucesfully completed\n";
            puts(replyMessage);
        } 
        else 
        {
            char replyMessage[100] = "Transfer Failed\nCheck permissions\n";
            puts(replyMessage);
        }
    }
    // If file entered is Marketing
    if(strcmp(FolderToSearchFor,"Marketing") == 0)
    {
        char command[500];
        int checker;
        strcpy(command, "cp /tmp/test.txt /home/fayezrahman/Desktop/serverFiles/Marketing");
        checker = system(command);
        if(checker == -1)
        {
            printf("System command failed\n");
        }

        if( access( "/home/fayezrahman/Desktop/serverFiles/Marketing/test.txt", F_OK ) != -1 ) 
        {
            char replyMessage[100] = "Transfer Sucesfully completed\n";
            puts(replyMessage);
        } 
        else 
        {
            char replyMessage[100]= "Transfer Failed\nCheck permissions\n";
            puts(replyMessage);
        }
    }

    // If file entered is Sales
    if(strcmp(FolderToSearchFor,"Root") == 0)
    {
        char command[500];
        int checker;
        strcpy(command, "cp /tmp/test.txt /home/fayezrahman/Desktop/serverFiles/root");
        system(command);
        if(checker == -1)
        {
            printf("System command failed\n");
        }

        // if the file exist( as in fully moved sucessfully)
        // if it doesn't exist it means they have no permission, so the file didn't move
        if( access( "/home/fayezrahman/Desktop/serverFiles/root/test.txt", F_OK ) != -1 ) 
        {
            char replyMessage[100] = "Transfer Sucesfully completed\n";
            puts(replyMessage);
        } 
        else 
        {
            char replyMessage[100] = "Transfer Failed\nCheck permissions\n";
            puts(replyMessage);
        }
    }
}

