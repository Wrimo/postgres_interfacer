
#include <iostream>
#include "code_gen.h"
#include <sstream>
#include <fstream>

#include <algorithm>
// https://stackoverflow.com/questions/5891610/how-to-remove-certain-characters-from-a-string-in-c
void dataCleanup(std::string &str)
{
    std::string badChars = "\"{}";
    for (unsigned int i = 0; i < str.length(); ++i)
        if (str[i] == ',')
            str[i] = ' ';

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

std::vector<VariableData> getVariablesAndTypes(std::string &str)
{
    dataCleanup(str);
    if (str.length() == 0)
        return {};
    std::istringstream ss(str);
    std::string name;
    std::string type;
    std::vector<VariableData> variables;
    while (!ss.eof())
    {
        ss >> name;
        ss >> type;
        VariableData s;
        s.name = name;
        s.type = convertDataType(type);
        variables.push_back(s);
    }
    return variables;
}

void generateStructs(pqxx::work &txn)
{
    std::cout << "GENERATING STRUCTS\n";
    pqxx::result r{txn.exec("SELECT * FROM private.get_tables_and_fields();")};
    std::ofstream headerFile("cpp/types.h");
    headerFile << "#include <string>\n";
    for (auto row : r)
    {
        headerFile << "\nstruct " << row["name"] << "{\n";

        std::string data = row["fields"].c_str();
        std::vector<VariableData> vars = getVariablesAndTypes(data);

        for (size_t i = 0; i < vars.size(); ++i)
        {
            headerFile << vars[i].type << " " << vars[i].name << ";\n";
        }

        headerFile << "};\n";
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

    headerFile << "std::string quote(std::string &);\n\n";
    cppFile << R"(std::string quote(std::string & str) { return "\'" + str + "\'";})"
            << "\n";

    for (auto row : r)
    {
        headerFile << "\npqxx::result " << row["name"].c_str() << "(pqxx::work &txn";
        cppFile << "\npqxx::result " << row["name"].c_str() << "(pqxx::work &txn";

        std::string vars = row["input"].c_str();
        std::vector<VariableData> params = getVariablesAndTypes(vars);

        for (size_t i = 0; i < params.size(); ++i)
        {
            headerFile << ", " << params[i].type;
            cppFile << ", " << params[i].type << " " << params[i].name;
        }
        headerFile << ");";
        cppFile << ")\n";

        std::ostringstream query;

        query << "SELECT * FROM " << row["name"].c_str();
        if (params.size() > 0)
        {
            query << "(\"";

            for (size_t i = 0; i < params.size(); ++i)
            {
                if (params[i].type == "std::string")
                {
                    query << " + quote(" << params[i].name << ")";
                }
                else if (params[i].type == "int" || params[i].type == "float")
                {
                    query << " + std::to_string(" << params[i].name << ")";
                }

                query << (i == params.size() -1 ? "" : R"(+ ", ")");
            }

            query << " + \");";
        }
        else
        {
            query << "()";
        }

        cppFile << "{\n	pqxx::result r {txn.exec(\"" << query.str() << "\")};";
        cppFile << "\n	return r;\n}";
    }
    std::cout << "FINISHED GENERATING FUNCTIONS\n";
}