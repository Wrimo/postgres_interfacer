#include <iostream>
#include "procedures.h"
#include "hidden.h"

int main() 
{
    std::stringstream conString; 
    conString << "postgresql://" << USERNAME << ":" << PASSWORD << "@localhost:" <<  PORT << "/" << USERNAME;
    pqxx::connection c{conString.str()};
    pqxx::work txn{c};

    pqxx::result r = get_authors(txn);
    for(auto row : r)
    {
        std::cout << row["firstName"] << "\n";
    }

    return 0;
}