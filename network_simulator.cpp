#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#define PORT 9090
#define INPUT_BUFFER_SIZE 2048//condition

using namespace std;
int currentClient;

class network_simulator
{
    private:
        fd_set networkfds;
        fd_set clientfds;

        uint16_t maxfd;

        int networksocket_fd; 
        int clientsocket_fd; 

        struct sockaddr_in servaddr;
        struct sockaddr_storage client_addr;

        char input_buffer[INPUT_BUFFER_SIZE];

        void (*newConnectionCallback) (uint16_t fd);
        void (*messageCallback) (uint16_t fd, char *buffer);
        void (*disconnectCallback) (uint16_t fd);
    public:
        network_simulator(int port){
            setup(port);
        }

        ~network_simulator(){
	        close(networksocket_fd);
        }

        void setup(int port){
            networksocket_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (networksocket_fd < 0) {
                perror("socket failed");
            }

            FD_ZERO(&networkfds);
            FD_ZERO(&clientfds);

            memset(&servaddr, 0, sizeof (servaddr)); 
            servaddr.sin_family = AF_INET;
            servaddr.sin_addr.s_addr = htons(INADDR_ANY);
            servaddr.sin_port = htons(port);

            bzero(input_buffer,INPUT_BUFFER_SIZE);
        }

        void initializeSocket(){
            int opt_value = 1;
            int ret_test = setsockopt(networksocket_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &opt_value, sizeof (int));
            if (ret_test < 0) {
                perror("setsockop failed");
                close(networksocket_fd);
            }
        }

        void bindSocket(){
            if (bind(networksocket_fd, (struct sockaddr*) &servaddr, sizeof (servaddr)) < 0) {
                perror("bind() failed");
            }
            FD_SET(networksocket_fd, &networkfds); 
            maxfd = networksocket_fd; 
        }

        void startListen(){
            if (listen(networksocket_fd, 3) < 0) {
                perror("listen() failed");
            }
        }

        void loop() {
            while (true)
            {
                clientfds = networkfds;
                  
                if (select(maxfd + 1, &clientfds, NULL, NULL, NULL) < 0) {
                    perror("select() failed");
                    close(networksocket_fd);
                }
                
                for (int i = 0; i < maxfd; i++) {
                    if (FD_ISSET(i, &clientfds)) { 
                        if (networksocket_fd == i) {
                            handleNewConnection();
                        } else {
                            recvInputFromExisting(i);
                        }
                    } 
                }

                bzero(&input_buffer,INPUT_BUFFER_SIZE);
            }
            
        }

        void init() {
            initializeSocket();
            bindSocket();
            startListen();
        }

        void handleNewConnection() {
            socklen_t addrlen = sizeof (client_addr);
                
            if ((clientsocket_fd = accept(networksocket_fd, (struct sockaddr*) &client_addr, &addrlen)) < 0) {
                perror("accept() failed");
            } else {    
                FD_SET(clientsocket_fd, &networkfds);
                if (clientsocket_fd > maxfd) {
                        maxfd = clientsocket_fd;
                }
            }
            newConnectionCallback(clientsocket_fd);
        }

        void recvInputFromExisting(int fd) {
            int nbytesrecv = recv(fd, input_buffer, INPUT_BUFFER_SIZE, 0);
            if (nbytesrecv <= 0){
                if (0 == nbytesrecv){
                    disconnectCallback((uint16_t)fd);
                    close(fd); 
                    FD_CLR(fd, &networkfds);
                    return;
                } else {
                    perror("recv() failed");
                }
                close(fd); 
                FD_CLR(fd, &networkfds); 
                return;
            }
            messageCallback(fd,input_buffer);
            bzero(&input_buffer,INPUT_BUFFER_SIZE);
        }

        void broadcastMessage(int fd, char *buffer){
            for()
        }

        void onMessage(void (*cb)(uint16_t fd, char *buffer)){
            messageCallback = cb;
        }

        void onConnect(void(*cb)(uint16_t)){
            newConnectionCallback = cb;
        }

        void onDisconnect(void(*cb)(uint16_t)){
            disconnectCallback = cb;
        }

};

struct data {
    char command; 
    char origin; 
    char dest;
};

void onConnectCb(uint16_t fd){
    cout<<"New client has been added\n";
}

void onDisconnectCb(uint16_t fd){
    cout<<"Client disconnected\n";
}

void onMessageCb(uint16_t fd, char *buffer){
    struct data _data;
    memcpy(&_data, buffer, sizeof(struct data));

    printf("got %c from %c to %c \n", _data.command, _data.origin, _data.dest);
}

int main(int argc, char const *argv[])
{
    network_simulator network(PORT);
    network.init();  
    network.onConnect(onConnectCb);
    network.onDisconnect(onDisconnectCb);
    network.onMessage(onMessageCb);
    network.loop();
    return 0;
}
