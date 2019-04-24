#include<stdio.h>
#include<string.h>    
#include<stdlib.h>    
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h>    
#include<pthread.h> 
#include <grp.h>
#include <pwd.h>

//global variables
#define PORT 8082
int userID;
int myGroupIds[50]; // temporary array to hold user group ID's

// declare function
void getGroupIDs(char *user);
void changeID(int myID);
void getFile(int sock);

pthread_mutex_t lock_x; // this is used for the mutex lock for shared resources
struct sockaddr_in cliaddr, servaddr; // global variables initliased so i can use them inside the server thread handler to determine who left!
char usernameFromClient[] = ""; 
void *server_handler (void *fd_pointer); // server handler function that will handle each thread coming in. 

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

	int sock = *(int *)fd_pointer;
    int read_size, write_size;
	static char client_message[2000];

    /*
    // get access to the lock
    pthread_mutex_lock(&lock_x);

    printf("PERFORM APPLICATION LOGIC HERE\n");

    printf("I CANT TALK IN THE SERVER BEFORE THIS MESSAGE EXECUTES\n");

    // release the lock 
    pthread_mutex_unlock(&lock_x);

    printf("I CAN TALK NOW IN THE SERVER\n");
    */


    // call function to receive file
    getFile(sock);
    
    
    while((read_size = recv(sock,client_message,2000,0)) > 0)
    {
        
        char userToSearchFor[100];
        char FolderToSearchFor[100];
        char FilePathToSearchFor[100];

        if(strcmp(client_message, "exit") == 0)
        {
            printf("Client %s:%d has exited \n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
            break;
        }

        /* This for loop here splits the strings received from the client based on WHITE SPACE characters*/
        /*
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
        printf("Strings or words after split by space are :\n");
        

        // after we splitted the message copy it to each string
        strcpy(FilePathToSearchFor,newString[0]);
        strcpy(FolderToSearchFor,newString[1]);
        strcpy(userToSearchFor,newString[2]);
        
        // check cooy has worked
        printf("%s\n",FilePathToSearchFor);
        printf("%s\n",FolderToSearchFor);
        printf("%s\n",userToSearchFor);
        */
        // call function to get user ID's
        //getGroupIDs(userToSearchFor);



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

    fprintf(stderr, "User : %s belongs in %d groups\n", user, ngroups);
     for (j = 0; j < ngroups; j++) 
     {
         printf("%d", groups[j]);
        myGroupIds[j] = groups[j]; // add each group ID to array so we can search for them later.
        gr = getgrgid(groups[j]);
        if (gr != NULL)
            printf(" (%s)", gr->gr_name);
        printf("\n");
    }

    // from the retrieved groups assign the ID of the user that is actually logged in on the client
    userID = myGroupIds[0];
    
    printf("User ID: %d\n",userID);
    //changeID(userID);
 
}

void changeID(int myID)
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

    setgroups(50, myGroupIds); // set 50 as size of the list
    if(setreuid(myID, uid) < 0)
    {
        printf("Error in changing REUID\n");
    }

    if(setregid(myID, gid) < 0)
    {
        printf("Error in changing REGID\n");
    }

    if(seteuid(myID) < 0)
    {
        printf("Error in changing EUID\n");
    }

    if(setegid(myID) < 0)
    {
        printf("Error in changing EGID\n");
    }



    printf("ID after it got changed\n");

    printf("User ID: %d\n",getuid());
    printf("Group ID: %d\n",getgid());
    printf("Effective User ID: %d\n",geteuid());
    printf("Effective User ID: %d\n",getegid());

    /*
    // attemp to copy
    char command[500];
    strcpy(command, "cp /tmp/test.txt /home/fayezrahman/Desktop/CA2/SystemSoftwareCA2/sales/test.txt");
    system(command);

    if (chown("/home/fayezrahman/Desktop/CA2/SystemSoftwareCA2/sales/test", userID, userID) == 01)
    {
    //die("chown fail");
    }
    */


    // set back to root
    int rootID = 0;
    setreuid(rootID, uid);
    setregid(rootID, gid);
    seteuid(rootID);
    setegid(rootID);

}

// ************************************ PUT THIS IN A SEPERATE FUNCTION LATER *********************************
     // search for group ID to check if user can access or not
     /*
    int found = 0;
    int searchfor = 1001;

   for (int i=0; i<ngroups; i++)
     {
        if(myGroupIds[i] == searchfor)
        {
            found = 1;
             break;
        }
    }

    if(found == 1)
     {
        printf("\n group number found, access allowed\n");
    }
    else
    {
        printf("\n %s is not part of the group folder\n", user);
    }
    */


void getFile(int sock)
{
    char file_buffer[512]; // Receiver buffer
    char* file_name = "/tmp/test.txt";
    FILE *file_open = fopen(file_name, "w");
        if(file_open == NULL)
            printf("File %s Cannot be opened file on server.\n", file_name);
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