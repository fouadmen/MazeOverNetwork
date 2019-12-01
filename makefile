all: simulator.exe

simulator.exe: simulator.o
	g++ -o simulator.exe simulator.o

simulator.o: simulator.cpp
	g++ -c simulator.cpp

clean:
	rm simulator.o simulator.exe
