
// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <iostream>
#include <termios.h>
#include <sys/ioctl.h>

#define PORT 9090
#define DATASIZE sizeof(struct data)

using namespace std;

struct data {
    char command; 
    char origin; 
    char dest;
};

int director_socket = 0; 

void socket_config(){
    struct sockaddr_in serv_addr; 
    //  feedback 
    if ((director_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        exit(EXIT_FAILURE);
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        perror("\nInvalid address/ Address not supported \n"); 
        exit(EXIT_FAILURE);
    } 
   
    if (connect(director_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        perror("\nConnection Failed \n");
        exit(EXIT_FAILURE);
    } 
}



int main(int argc, char const *argv[]) 
{ 
    struct data _data;
    socket_config();
    _data.origin = 'd'; //director
    _data.dest = 'm'; //maze

    while (true)
    {
        cin>>_data.command;
        char cmd[DATASIZE];
        memcpy((void*)cmd, (void*)&_data, DATASIZE);
        write(director_socket , cmd , DATASIZE);   
    }
    
    return 0; 
} 
