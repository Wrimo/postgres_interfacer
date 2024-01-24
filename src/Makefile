gen: main.cpp code_gen.h code_gen.cpp code_gen_types.h
	g++ -o gen -Wall -O2 main.cpp code_gen.cpp -lpqxx -lpq
query: query.cpp procedures.h types.h 
	g++ -o query -Wall -O2 query.cpp -lpqxx -lpq
clean:
	rm -f gen procedures.h procedures.cpp types.h query
