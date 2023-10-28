#include <iostream>
#include "procedures.h"
#include "hidden.h"

int main()
{
    std::stringstream conString;
    conString << "postgresql://" << USERNAME << ":" << PASSWORD << "@localhost:" << PORT << "/" << USERNAME;
    pqxx::connection c{conString.str()};
    pqxx::work txn{c};

    pqxx::result r = get_authors_over_age(txn, 80);
    for (auto row : r)
    {
        std::cout << row["firstName"] << " " << row["lastName"] << "\n";
    }

    return 0;
}