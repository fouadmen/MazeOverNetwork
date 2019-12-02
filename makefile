all: simulator.exe director.exe network_simulator.exe

simulator.exe: simulator.o
	g++ -o simulator.exe simulator.o -lpthread

simulator.o: simulator.cpp
	g++ -c simulator.cpp 

director.exe: director.o
	g++ -o director.exe director.o

director.o: director.cpp
	g++ -c director.cpp

network_simulator.exe: network_simulator.o
	g++ -o network_simulator.exe network_simulator.o

network_simulator.o: network_simulator.cpp
	g++ -c network_simulator.cpp

clean:
	rm simulator.o simulator.exe director.o director.exe network_simulator.o network_simulator.exe
