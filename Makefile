all: server.exe

server.exe: src/server.c
	gcc src/server.c -o server.exe

dns.exe: src/dns.c
	gcc src/dns.c -o dns.exe
