#include "code_gen.h"
#include <iostream>
#include <string>
#include <sstream>

void dataCleanup(std::string & str)
{
    throw "not implemented";
}

void generateTypes(pqxx::work &txn)
{
    pqxx::result r {txn.exec("SELECT name, fields FROM get_tables_and_fields();")};
    std::stringstream headerFile; 
    for (auto row : r)
    {
        headerFile << "struct {\n";
        std::string data = row["fields"].c_str();
        dataCleanup(data);
        std::istringstream ss(data);
        
        std::string name = "";
        std::string type = "";
        while(!ss.eof())
        {
            ss >> name;
            ss >> type; 
            headerFile << type << " " << name << ";\n";
        }
        headerFile << "} " << row["name"] << ";\n"; 
    }
    std::cout << headerFile.str();
}