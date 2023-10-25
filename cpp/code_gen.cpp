#include "code_gen.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

// https://stackoverflow.com/questions/5891610/how-to-remove-certain-characters-from-a-string-in-c
void dataCleanup(std::string &str)
{
    std::string badChars = "\"{},";
    // for (unsigned int i = 0; i < str.length(); ++i)
    //     if (str[i] == ',')
    //         str[i] = ' ';

    for (unsigned int i = 0; i < badChars.length(); ++i)
        str.erase(remove(str.begin(), str.end(), badChars[i]), str.end());
}

std::string convertDataType(std::string &postgresType)
{
    if (postgresType == "text")
        return "std::string";
    else if (postgresType == "integer")
        return "int";
    else if (postgresType == "numeric")
        return "double";
    else if (postgresType == "boolean")
        return "bool";
    std::cout << postgresType << " is not a valid type\n";
    return "void *";
}

std::string getTypes(std::string &str, std::string delimeter)
{
    dataCleanup(str);
    if (str.length() == 0)
        return "";
    std::ostringstream out;
    std::istringstream ss(str);
    std::string name;
    std::string type;
    while (!ss.eof())
    {
        ss >> name;
        ss >> type;
        type = convertDataType(type);
        if(ss.eof())
        {
            out << type << " " << name;
            break;
        }
        out << type << " " << name << delimeter;
    }
    return out.str();
}

void generateStructs(pqxx::work &txn)
{
    std::cout << "GENERATING STRUCTS\n";
    pqxx::result r{txn.exec("SELECT * FROM private.get_tables_and_fields();")};
    std::ofstream headerFile("cpp/types.h");
    headerFile << "#include <string>\n";
    for (auto row : r)
    {
        headerFile << "\nstruct {\n";

        std::string data = row["fields"].c_str();
        headerFile << getTypes(data, ";\n");

        headerFile << "} " << row["name"] << ";\n";
    }
    headerFile.close();
    std::cout << "FINISHED GENERATING TYPES\n";
}

void generateFunctions(pqxx::work &txn)
{
    std::cout << "GENERATING FUNCTIONS\n";
    pqxx::result r{txn.exec("SELECT * FROM private.get_stored_procedures();")};
    std::ofstream headerFile("cpp/procedures.h");
    std::ofstream cppFile("cpp/procedures.cpp");

    headerFile << "#include <pqxx/pqxx>\n"
               << "#include <string>\n\n";
    cppFile << "#include \"procedures.h\"\n";

    for (auto row : r)
    {
        headerFile << "\npqxx::result " << row["name"].c_str() << "(pqxx::work &txn"; // need to modify to have support for optional parameters, requires changes to SQL
        cppFile << "\npqxx::result " << row["name"].c_str() << "(pqxx::work &txn";

        std::string raw = row["input"].c_str();
        std::string params = getTypes(raw, ", ");

        if(params != "")
        {
            headerFile << ", " << params;
            cppFile << ", " << params;
        }

        headerFile << ");";
        cppFile << ")\n";
        cppFile << "{\n	pqxx::result r {txn.exec(\"SELECT * FROM " << row["name"].c_str() << "();\")};";
        cppFile << "\n	return r;\n}";
    }
    std::cout << "FINISHED GENERATING FUNCTIONS\n";
}