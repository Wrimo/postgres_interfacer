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

    insert_author(txn, "Fyodor", "Dostoevsky", 100);
    auto r = get_authors(txn);
    for (auto i : r) 
    {
        std::cout << i.firstname << " " << i.lastname << "\n";
    }
    txn.commit();

    return 0;
}