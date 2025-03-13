all: ringmaster player

ringmaster: ringmaster.cpp server.cpp client.cpp potato.h helper.h
	g++ -g -o ringmaster ringmaster.cpp server.cpp client.cpp
player: player.cpp server.cpp client.cpp potato.h helper.h
	g++ -g -o player player.cpp server.cpp client.cpp

.PHONY:
	clean
clean:
	rm -rf *.o ringmaster player