#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <thread>

#define AGENT '*'
#define FPS 30
#define PORT 9091

using namespace std;

vector<vector<bool>> MAZE;
//todo : create state vector
int agentPos[2] = {0, 0};
int startPos[2] = {0, 0};
int endPos[2] = {0, 0};

//network

int simulator_fd, simulator_socket;
struct sockaddr_in address; 
char command[1] = {0}; 
bool socket_config_done = false; 



void createMaze() {
  ifstream configFile("simulator.conf", ios::in);//json xml
  if (configFile.is_open()) {
    string line;

    int i = 0;
    while (getline(configFile, line)) {
      vector<bool> tmp;
      for (size_t j = 0; j < line.size(); j++) {
          if(line[j]=='w'){
            tmp.push_back(true);
          }
          else{
            if (line[j]=='s') {
              startPos[1] = i;
              startPos[0] = j;
              agentPos[1] = i;
              agentPos[0] = j;
            } else if(line[j]=='e') {
              endPos[1] = i;
              endPos[0] = j;
            }
            tmp.push_back(false);
          }
      }
      MAZE.push_back(tmp);
      ++i;
    }
    configFile.close();
  }
  else {
    cerr << "Unable to open file\n";
  }

}

void drawMaze() {
  for (size_t y = 0; y < MAZE.size(); y++) {
    for (size_t x = 0; x < MAZE[y].size(); x++) {
        if (MAZE[y][x]){
          printf("#");
        } else if(agentPos[0] == x && agentPos[1] == y) {
          printf("%c", AGENT);
        } else {
          printf(" ");
        }
    }
    printf("\n");
  }
}

void moveAgent(const char dir) {
  int x = agentPos[0];
  int y = agentPos[1];
  
  if (dir == 'a' && x-1 >= 0 && !MAZE[y][x-1]) {
    --agentPos[0];
  } else if (dir == 'w' && y-1>=0 && !MAZE[y-1][x]) {
    --agentPos[1];
  } else if (dir == 'd' && x+1 < MAZE[0].size() && !MAZE[y][x+1]) {
    ++agentPos[0];
  } else if (dir == 's' && y+1 < MAZE.size() && !MAZE[y+1][x]) {
    ++agentPos[1];
  }
  
}

void setupConnection(){
  const int addrlen = sizeof(address);
  if ((simulator_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
    perror("socket failed"); 
    exit(EXIT_FAILURE); 
  } 
      
  address.sin_family = AF_INET; 
  address.sin_addr.s_addr = INADDR_ANY; 
  address.sin_port = htons( PORT ); 
      
  if (bind(simulator_fd, (struct sockaddr *)&address,sizeof(address))<0) { 
    perror("binding failed"); 
  } 

  if (listen(simulator_fd, 3) < 0) { 
    perror("listen"); 
  } 

  if ((simulator_socket = accept(simulator_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0) { 
    perror("accept"); 
  } 
  socket_config_done = true;
}

bool readCommand(){
  read(simulator_socket , command, 1);
  return true;  
}

void mainLoop(){
  char key;
  while(true){
    drawMaze();
    cout.flush();
    if(socket_config_done && readCommand()){
      moveAgent(command[0]);
    }      
    usleep(round(1000000/FPS));
    system("clear");
  }
}

int main(int argc, char const *argv[]) {
  thread network_config_t(setupConnection);
  //case client left
  //explore the maze
  createMaze();
  mainLoop();
  network_config_t.join();
  return 0;
}
