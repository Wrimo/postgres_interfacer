pinter: main.cpp code_gen.h code_gen.cpp code_gen_types.h
	g++ -o pinter -Wall -O2 main.cpp code_gen.cpp -lpqxx -lpq
query: query.cpp generated_procedures.h generated_types.h 
	g++ -o query -Wall -O2 query.cpp -lpqxx -lpq
clean:
	rm -f pinter generated_procedures.*  generated_types.* query
