/*
Example of a program using generated functions. 
Will not work until code has been generated for the provided example SQL. 
*/

#include <iostream>
#include <vector>
#include "procedures.h"
#include "hidden.h"


int main()
{
    std::stringstream conString;
    conString << "postgresql://" << USERNAME << ":" << PASSWORD << "@localhost:" << PORT << "/" << USERNAME;
    pqxx::connection c{conString.str()};
    pqxx::work txn{c};

    auto r = get_author_books(txn);
    for (auto i : r) 
    {
        std::cout << i.name << " wrote " << i.title << "\n";
    }

    return 0;
}