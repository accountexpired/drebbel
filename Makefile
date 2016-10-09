drebbel: drebbel.o
	clang++ drebbel.o -o drebbel -lsfml-graphics -lsfml-window -lsfml-system

drebbel.o: drebbel.cpp
	clang++ -c drebbel.cpp
