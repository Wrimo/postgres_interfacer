
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "code_gen.h"

std::map<std::string, TableData> Tables;

void dataCleanup(std::string &str)
{
    if (str.substr(0, 5) == "TABLE") 
        str.erase(0, 5); 

    for (unsigned int i = 0; i < str.length(); ++i)
        if (str[i] == ',')
            str[i] = ' ';

    std::string badChars = "\"{}()";
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

std::string generateStructs(std::string &name, std::string &colums)
{
    std::ostringstream strStruct;

    strStruct << "\nstruct " << name << "Data{\n";

    TableData table;
    table.name = name + "Data";

    std::vector<VariableData> vars = getVariablesAndTypes(colums);
    table.rows = vars;
    Tables[table.name] = table;

    for (size_t i = 0; i < vars.size(); ++i)
    {
        strStruct << vars[i].type << " " << vars[i].name << ";\n";
    }
    strStruct << "};\n";

    return "\n" + strStruct.str();
}

void generateFunctionStructs(pqxx::work &txn, std::string name, std::string columns)
{
    std::ofstream headerFile("types.h", std::ios_base::app);
    headerFile << generateStructs(name, columns);
    headerFile.close();
}

void generateTableStructs(pqxx::work &txn)
{
    std::cout << "GENERATING TABLE STRUCTS\n";
    pqxx::result r{txn.exec("SELECT * FROM private.get_tables_and_fields();")};
    std::ofstream headerFile("types.h");
    headerFile << "#include <string>\n";
    for (auto row : r)
    {
        std::string name = row["name"].c_str();
        std::string columns = row["fields"].c_str();
        headerFile << generateStructs(name, columns);
    }
    headerFile.close();
    std::cout << "FINISHED TABLE STRUCTS\n";
}

void generateFunctions(pqxx::work &txn)
{
    std::cout << "GENERATING FUNCTIONS\n";
    pqxx::result r{txn.exec("SELECT * FROM private.get_stored_procedures();")};
    std::ofstream headerFile("procedures.h");
    std::ofstream cppFile("procedures.cpp");

    headerFile << "#include <pqxx/pqxx>\n"
               << "#include <string>\n"
               << "#include <vector>\n"
               << "#include \"types.h\"\n\n";
    cppFile << "#include \"procedures.h\"\n";

    headerFile << "std::string quote(std::string &);\n\n";
    cppFile << R"(std::string quote(std::string & str) { return "\'" + str + "\'";})"
            << "\n\n";

    for (auto row : r)
    {
        std::istringstream returnType(row["output"].c_str());
        std::string table;

        returnType >> table;
        if (table == "SETOF") // is returning rows from an existing table
            returnType >> table;
        else if (table.substr(0, 5) == "TABLE") // is returning a virtual table that needs to be created
        {
            table = row["name"].c_str();
            generateFunctionStructs(txn, table, row["output"].c_str());
        }
        table = table + "Data";

        headerFile << "\nstd::vector<" << table << "> " << row["name"].c_str() << "(pqxx::work &txn";
        cppFile << "\nstd::vector<" << table << "> " << row["name"].c_str() << "(pqxx::work &txn";

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
                    query << " + quote(" << params[i].name << ")";
                else if (params[i].type == "int" || params[i].type == "float")
                    query << " + std::to_string(" << params[i].name << ")";

                query << (i == params.size() - 1 ? "" : R"(+ ", ")");
            }

            query << " + \");";
        }
        else
        {
            query << "()";
        }

        cppFile << "{\n	pqxx::result r {txn.exec(\"" << query.str() << "\")};\n";

        cppFile << "    std::vector<" << table << "> data;\n";
        cppFile << "    for (auto row : r)\n    {\n"
                << "        " << table << " x;\n";

        // convert each value in row to value in struct
        for (VariableData row : Tables[table].rows)
        {
            if (row.type == "int")
                cppFile << "        x." << row.name << " = "
                        << "std::stoi(row[\"" << row.name << "\"].c_str());\n";
            else if (row.type == "float")
                cppFile << "        x." << row.name << " = "
                        << "std::stof(row[\"" << row.name << "\"].c_str());\n";
            else
                cppFile << "        x." << row.name << " = "
                        << "row[\"" << row.name << "\"].c_str();\n";
        }
        cppFile << "        data.push_back(x);\n";
        cppFile << "    }";

        cppFile << "\n	return data;\n}";
    }
    cppFile.close();
    std::cout << "FINISHED GENERATING FUNCTIONS\n";
}