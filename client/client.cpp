// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#define _OE_SOCKETS
#define PORT 8080
using namespace std;

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    // char *hello = "listall";
    while(1)
    {
		char buffer[1024];
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			printf("\n Socket creation error \n");
			return -1;
		}
		// printf("lolololol1");

		memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
													// which is meant to be, and rest is defined below

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(PORT);
		// printf("lolololol2");

		// Converts an IP address in numbers-and-dots notation into either a
		// struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
		if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
		{
			printf("\nInvalid address/ Address not supported \n");
			return -1;
		}
		// printf("lolololol3");
		if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
		{
			printf("\nConnection Failed \n");
			return -1;
		}
		else  	printf("Connection established!\n");


	    printf("## Name of the file(to see available files, use \"listall\":\n>>> ");
	    char filename[128];
	    scanf("%[^\n]s", filename);
	    getchar();
	    // printf("Sent: %s\n", filename);
	    // Send the request to download the file to the server.
	    send(sock, filename, strlen(filename) , 0 );  
	    // Further writes are disallowed to socket associated with file descriptor 'sock'
	    // shutdown(sock, SHUT_WR); 
	    char* array[51];
        for(int i=0;i<51;i++) array[i] = NULL;
        // printf("abc\n");
        char *p = strtok(filename, " \t");
        // printf("%s\n", p); 
        int i = 0;
        while(p != NULL){
            // printf("%s\n", p); 
            array[i++] = p;
            p = strtok(NULL, " \t");
        }


	    if(!strcmp(array[0], "send"))
		{
		    printf("File %s requested from server\n", array[1]);
	 	    // Create a file with permissions 664 and assign file descriptor to fd
	 	    int l = read(sock, buffer, 1024);

			if(l == -1){
				printf("Error in getting size of file from server.\n");
				// return -1
				continue;
			}

			if(buffer[0] == '-'){
				printf("Error occured %s\n", &buffer[3]);
				// return -1;
				continue;
			}

			int sizeOfFile = 0, done = 0, s = 0;

			for(int i=0; i < l; ++i){
				if(!done && buffer[i] == '\n') done = i;
				else if(done){
					buffer[i - done - 1] = buffer[i];
					s = i - done;
				}
				else sizeOfFile = (sizeOfFile * 10) + buffer[i] - '0';
			}

			buffer[s] = '\0';

			int fileWrite = open(array[1], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

			int w = write(fileWrite, buffer, s);
			int sizeRead = 0;

			if(w == -1){
				printf("Error in writing to file.\n");
				return -1;
			}
			else sizeRead += w;
			
			if(fileWrite == -1){
				printf("Cannot open file on client.\n");
				return -1;
			}

			while(sizeRead < sizeOfFile){
				int nowRead = read(sock, buffer, 1024);
				if(nowRead == -1){
					printf("Error in getting file content from server.\n");
					return -1;		
				}

				int t = write(fileWrite, buffer, nowRead);
				if(t == -1 || t != nowRead){
					printf("Error in writing to file.\n");
					return -1;
				}

				sizeRead += nowRead;
			}
			printf("File %s received successfully!\n", array[1]);
			close(fileWrite);
	 	}
	 	else if(!strcmp(filename,"listall"))
	 	{
	 		// printf("Printing ls\n");
	 		valread = read( sock , buffer, 1024);  // receive message back from server, into the buffer
        	printf("%s\n\n",buffer );
	 	}
	}
	close(sock);

    return 0;
}
