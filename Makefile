drebbel: drebbel.o
	clang++ drebbel.o -o drebbel -lsfml-graphics -lsfml-window -lsfml-system -std=c++14

drebbel.o: drebbel.cpp
	clang++ -c drebbel.cpp -std=c++14
