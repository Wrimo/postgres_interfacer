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

    std::vector<author> r = get_author_name_like(txn, "Vir");
    for (auto i : r) 
    {
        std::cout << i.firstname << " " << i.lastname << "\n";
    }

    return 0;
}