#include "code_gen.h"
#include <iostream>
#include <string>

void generateTypes(pqxx::work &txn)
{
    pqxx::result r {txn.exec("SELECT name, fields FROM get_tables_and_fields();")};
    for (auto row : r)
    {
        std::cout << row["name"] << ": " << row["fields"] << "\n";
    }
}