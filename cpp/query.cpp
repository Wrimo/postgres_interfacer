#include <iostream>
#include <vector>
#include "procedures.h"
#include "types.h"
#include "hidden.h"


int main()
{
    std::stringstream conString;
    conString << "postgresql://" << USERNAME << ":" << PASSWORD << "@localhost:" << PORT << "/" << USERNAME;
    pqxx::connection c{conString.str()};
    pqxx::work txn{c};

    pqxx::result r = get_author_name_like(txn, "Cormac");
    std::vector<author> vec; 
    for (auto row : r)
    {
        author a; 
        a.id = std::stoi(row["id"].c_str());
        a.firstname = row["firstName"].c_str();
        a.lastname = row["lastName"].c_str();
        a.age = std::stoi(row["age"].c_str());
        vec.push_back(a);
    }
    for (auto i : vec) 
    {
        std::cout << i.firstname << " " << i.lastname << "\n";
    }

    return 0;
}