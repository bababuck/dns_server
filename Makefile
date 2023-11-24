all:

%: src/%.cc
	g++ -std=c++11 $< -o %.exe

%: src/%.c
	gcc $< -o %.exe
