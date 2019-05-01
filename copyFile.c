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
#include <dirent.h>
#include "functions.h"

//functions
void checkIfFileMoved(int sock, char *file);
//global variable
char filetoTransfer[500] = "/home/fayezrahman/Desktop/serverFiles/";


void copyFileToDestination(int sock, char *FolderToSearchFor)
{
    // append the folder that the user has selected  to the full path declared 
    // as the global variable above
    strcat(filetoTransfer,FolderToSearchFor);
    printf("Copying file to this destination ->: %s\n",filetoTransfer);


    int checker;
    char copyCommand[500] = "cp /tmp/test.txt ";
    strcat(copyCommand, filetoTransfer);
    checker = system(copyCommand);
    if(checker == -1)
    {
        printf("System command failed\n");
    }

    // call our functions to check if the file has moved or not
    checkIfFileMoved(sock, filetoTransfer);


    // This for loop will then remove the folder word from the string above
    // so that the next folder will get appended as normal 
    int i, j = 0, k = 0,n = 0;
    int flag = 0;
    for(i = 0 ; filetoTransfer[i] != '\0' ; i++)
    {
        k = i;
     
        while(filetoTransfer[i] == FolderToSearchFor[j])
        {
            i++,j++;
            if(j == strlen(FolderToSearchFor))
            {
                flag = 1;
                break;
            }
        }
    j = 0;

    if(flag == 0)
        i = k;      
    else
        flag = 0;

    filetoTransfer[n++] = filetoTransfer[i];
    }

    filetoTransfer[n] = '\0';

    printf("File after word has been removed : %s\n",filetoTransfer);
}
/* This functions checks if the file has moved successfully to the directory or not
if it has moved to the directory it will print out a message saying transfer completed
if it has not moved it will print out saying transfer failed check your permissions*/
void checkIfFileMoved(int sock, char *file)
{
    DIR *d;
    struct dirent *dir;
    d = opendir(file);
    if (d == NULL)
    {
        char replyMessage[100] = "Transfer Sucesfully completed\n";
        puts(replyMessage);
        write(sock,replyMessage,strlen(replyMessage));
    }
    else
    {
        char replyMessage[100] = "Transfer Failed\nCheck permissions\n";
        puts(replyMessage);
        write(sock,replyMessage,strlen(replyMessage));
    }
}