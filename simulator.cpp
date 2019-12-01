#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <cstring>

using namespace std;

const int SIZE = 20;
const char* AGENT = ".";
const char* CLEAR = "clear";
const int fps = 30;

vector<vector<bool>> MAZE;
//todo : create state vector
int agentPos[2] = {8, SIZE/2};
int startPos[2] = {0, 0};
int endPos[2] = {0, 0};

void createMaze() {
  ifstream configFile("simulator.conf", ios::in);//json xml
  if (configFile.is_open()) {
    string line;
    string wall("w");
    string start("s");
    string end("e");

    int i = 0;
    while (getline(configFile, line)) {
      vector<bool> tmp;
      for (size_t j = 0; j < line.size(); j++) {
          if(wall.compare(line.substr(j,1)) == 0){
            tmp.push_back(true);
          }
          else{
            if (start.compare(line.substr(j,1)) == 0) {
              startPos[0] = i;
              startPos[1] = j;
              agentPos[0] = i;
              agentPos[1] = j;
            } else if(end.compare(&line[j]) == 0) {
              endPos[0] = i;
              endPos[1] = j;
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

void drawMaze(int *agentPos) {
  for (size_t i = 0; i < MAZE.size(); i++) {
    for (size_t j = 0; j < MAZE[i].size(); j++) {
        if (MAZE[i][j]){
          printf("#");
        } else if(agentPos[0] == i && agentPos[1] == j){
          printf("%s", AGENT);
        } else {
          printf(" ");
        }
    }
    printf("\n");
  }
}

void getInput() {
  // q,z,d,s
  int input = 0;
  scanf("%d", &input);
  int x = agentPos[0];
  int y = agentPos[1];
  switch (input) {
    case 4:
      if (x-1 >= 0 && !MAZE[x-1][y]) {
        agentPos[0] -= 1;
      }
      break;
    case 8:
      if (y-1>=0 && !MAZE[x][y-1]) {
        agentPos[1] -= 1;
      }
      break;
    case 6:
      if (x+1 < MAZE[0].size() && !MAZE[x+1][y]) {
        agentPos[0] += 1;
      }
      break;
    case 5:
      if (y+1 < MAZE.size() && !MAZE[x][y+1]) {
        agentPos[1] += 1;
      }
      break;
  }
}

void mainLoop(){
    while(true){
      drawMaze(agentPos);
      cout.flush();
      usleep(round(1000000/fps));
      getInput();
      system("clear");
  }
}

int main(int argc, char const *argv[]) {
  createMaze();
  mainLoop();
  return 0;
}
