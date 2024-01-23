#include <iostream>
#include <sstream>
#include <fstream>
#include "hidden.h"
#include "code_gen.h"

int main(int argc, char *argv[])
{
    std::stringstream conString;
    conString << "postgresql://" << USERNAME << ":" << PASSWORD << "@localhost:" << PORT << "/" << USERNAME;
    pqxx::connection c{conString.str()};
    pqxx::work txn{c};

    if (argc == 2 && std::string(argv[1]) == "init")
    {
        std::ifstream procedureScript("private_procedures.sql");
        std::string command((std::istreambuf_iterator<char>(procedureScript)),
                           std::istreambuf_iterator<char>());
        txn.exec(command);
        txn.commit();
    }
    else if (argc > 1)
    {
        std::cout << "Bad argument " << std::string(argv[1]) << "\n";
        return 1;
    }

    generateTableStructs(txn);
    generateStoredProcedures(txn);


    return 0;
}