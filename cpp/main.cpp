#include <iostream> 
#include <sstream>
#include "hidden.h"
#include "code_gen.h"

int main() 
{   
    std::stringstream conString; 
    conString << "postgresql://" << USERNAME << ":" << PASSWORD << "@localhost:" <<  PORT << "/" << USERNAME;
    pqxx::connection c{conString.str()};
    pqxx::work txn{c};

    generateStructs(txn);
    generateFunctions(txn);
        
    txn.commit();

    return 0; 
}