#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#define PORT 9092

#define INPUT_BUFFER_SIZE 100

class network_simulator
{
private:
    fd_set network_socket_fds;
    fd_set client_fds;
    int network_socket_fd, network_socket, new_client;
    struct sockaddr_in address;
    struct sockaddr_storage client_addr;
    char input_buffer[INPUT_BUFFER_SIZE];
    int maxfd;
public:
    network_simulator(int port){
        setup(port);
    }

    ~network_simulator(){
        close(network_socket);
    }

    void setup(int port){

        if ((network_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
            perror("socket failed"); 
            exit(EXIT_FAILURE); 
        } 

        FD_ZERO(&network_socket_fds);
        FD_ZERO(&client_fds);

        memset(&address, 0, sizeof (address));
            
        address.sin_family = AF_INET; 
        address.sin_addr.s_addr = INADDR_ANY; 
        address.sin_port = htons( PORT );

    }

    void bindSocket(){
        if(bind(network_socket_fd, (struct sockaddr*) &address, sizeof (address)) < 0){
            perror("binding failed"); 
            exit(EXIT_FAILURE); 
        }
        FD_SET(network_socket_fd, &network_socket_fds);
        maxfd = network_socket_fd;
    }

    void startListen(){
        if (listen(network_socket_fd, 2) < 0) { 
            perror("listen failed"); 
            exit(EXIT_FAILURE); 
        } 
    }

    void initNetwork(){
        bindSocket();
        startListen();
    }

    void loop(){
        if(select(maxfd + 1, &client_fds, NULL, NULL, NULL) < 0){
            perror("select failed"); 
            close(network_socket_fd);
            exit(EXIT_FAILURE); 
        }

        for (int i = 0; i <= maxfd; i++) {
            if (FD_ISSET(i, &client_fds)) { 
                if (network_socket_fd == i) {
                    handleNewClient();
                } else {
                    recvData(i);
                }
            } //loop on to see if there is more
        }
    }


    void handleNewClient(){
        
    	socklen_t addrlen = sizeof (client_addr);
    	new_client = accept(network_socket, (struct sockaddr*) &client_addr, &addrlen);
    	
	    if (new_client < 0) {
        	perror("accept failed");
	    } else {
        	FD_SET(new_client, &network_socket_fds);
        	if (new_client > maxfd) {
            		maxfd = new_client;
        	}
        	
        }
        newClientCallback(new_client); //call the callback
    }

    void recvData(int fd) {
        int nbytesrecv = recv(fd, input_buffer, INPUT_BUFFER_SIZE, 0);
        if (nbytesrecv <= 0)
        {
            //problem
            if (0 == nbytesrecv)
        {
        	disconnectCallback((uint16_t)fd);
            close(fd); 
                FD_CLR(fd, &masterfds);
                return;
        } else {
        	perror("[SERVER] [ERROR] recv() failed");
        }
        close(fd); //close connection to client
        FD_CLR(fd, &masterfds); //clear the client fd from fd set
        return;
    }
    #ifdef SERVER_DEBUG
    printf("[SERVER] [RECV] Received '%s' from client!\n", input_buffer);
    #endif
    receiveCallback(fd,input_buffer);
    //memset(&input_buffer, 0, INPUT_BUFFER_SIZE); //zero buffer //bzero
    bzero(&input_buffer,INPUT_BUFFER_SIZE); //clear input buffer
    }

    void newClientCallback(int);
};




int main(int argc, char const *argv[])
{/*
    struct sockaddr_in address; 
    const int addrlen = sizeof(address);
    int network_fd;
    int network_socket;

    if ((network_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
        
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
        
    if (bind(network_fd, (struct sockaddr *)&address,sizeof(address))<0) { 
        perror("binding failed"); 
    } 

    

    if ((network_socket = accept(network_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0) { 
        perror("accept"); 
    } 
*/
    return 0;
}
