all: server.exe

server.exe: src/server.c
	gcc src/server.c -o server.exe
