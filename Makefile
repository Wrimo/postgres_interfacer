a.out: cpp/main.cpp cpp/code_gen.h cpp/code_gen.cpp
	g++ -Wall -O2 cpp/main.cpp cpp/code_gen.cpp -lpqxx -lpq
clean:
	rm -f a.out
