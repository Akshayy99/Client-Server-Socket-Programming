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
#include <bits/stdc++.h>
#define _OE_SOCKETS
#define PORT 8080
using namespace std;


int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread, file_fd, pid;
    struct sockaddr_in address;  // sockaddr_in - references elements of the socket address. "in" for internet
    int opt = 1;
    int addrlen = sizeof(address);
    // send(new_socket , hello , strlen(hello) , 
    // char buffer[1024] = {0};
    char ls_buffer[1024] = {0};
    char filename[128] = {0};
    string response = "Success";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc.
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening...\n");
    

    while(1)
    {
        if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
        if((pid == fork()) == -1)
        {
            close(new_socket);
            continue;
        }
        else if(pid > 0)
        {
            close(new_socket);
            printf("parent\n");
            continue;
        }
        else if(pid == 0)
        {
            char buffer[1024]={0};
            valread = read( new_socket , buffer, 1024);
            // printf("Buffer recd is: %s\n", buffer);
            // buffer[valread] = '\0';
            // printf("Reading\n");
            char* array[51];
            for(int i=0;i<51;i++) array[i] = NULL;
            // printf("abc\n");
            char *p = strtok(buffer, " \t");
            // printf("%s\n", p); 
            int i = 0;
            while(p != NULL){
                // printf("%s\n", p); 
                array[i++] = p;
                p = strtok(NULL, " \t");
            }

            char buf[512] = {0};
            if(!strcmp(array[0], "listall")) 
            {
                DIR *mydir;
                struct dirent *myfile;
                mydir = opendir(".");
                while((myfile = readdir(mydir)) != NULL)
                {
                    if(myfile->d_name[0] == '.') continue;
                    sprintf(buf, "%s\n%s", buf, myfile->d_name);
                }
                send(new_socket, buf, 512, 0);
            }
            if(!strcmp(array[0], "send"))
            {
                int fd = open(array[1], O_RDONLY);
                if(fd < 0) {
                    perror("open failed");
                    string str = to_string(-1) + '\n';
                    str += string(strerror(errno));
                    send(new_socket , str.c_str() , strlen(str.c_str()) , 0);
                    // exit(EXIT_FAILURE);
                    continue;
                }
                struct stat st;
                if(stat(array[1], &st) < 0) {
                    perror("stat failed");
                    string str = to_string(-1) + '\n';
                    str += string(strerror(errno));
                    send(new_socket , str.c_str() , strlen(str.c_str()) , 0);
                    // exit(EXIT_FAILURE);
                    continue;
                }

                string str = to_string(st.st_size) + '\n';
                send(new_socket , str.c_str() , strlen(str.c_str()) , 0);

                while (1) 
                {
                    int bytes_left = read(fd, buffer, 1024);
                    if(bytes_left == 0) {
                        break;
                    }
                    if(bytes_left < 0) {
                        perror("read failed");
                        exit(EXIT_FAILURE);
                    }

                    char* p = buffer;
                    while(bytes_left > 0) {
                        int bytes_written = write(new_socket, p, bytes_left);
                        if (bytes_written <= 0) {
                            perror("write failed");
                            exit(EXIT_FAILURE);
                        }
                        bytes_left -= bytes_written;
                        p += bytes_written;
                    }
                }
                close(fd);
            }
            else if(strcmp(array[0], "send") && strcmp(array[0], "listall"))
            {
                printf("Usage: send [filename]\n");
                continue;
            }
        }

            
    }
    close(new_socket);
    close(server_fd);










    return 0;
}
