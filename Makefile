CC=g++
CXX=g++
CPPFLAGES=-O2 -pthread -pipe -std=c++11 -Wall
LDFLAGS=-std=c++11

all: test
	./bin/test 2>/dev/null

computer:
	$(CC) $(CPPFLAGES) -o bin/$@ src/main_computer.cpp lib/libplayer.a

player:
	$(CC) $(CPPFLAGES) -o bin/$@ src/main_player.cpp lib/libplayer.a

check_%:
	$(CC) $(CPPFLAGES) -o bin/$@ src/$@.cpp lib/libplayer.a
