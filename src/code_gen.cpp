
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "code_gen.h"

std::map<std::string, TableData> Tables;
LanguageImplementation *langDef;

void codeGenStart(LanguageImplementation *lang, pqxx::work &txn)
{
    langDef = lang;
    generateTableStructs(txn);
    generateStoredProcedures(txn);
}

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
        s.type = langDef->convertType(type);
        variables.push_back(s);
    }
    return variables;
}

std::string generateStructs(std::string &name, std::string &colums)
{
    std::ostringstream strStruct;

    TableData table;
    table.name = name + "Data";

    std::vector<VariableData> vars = getVariablesAndTypes(colums);
    table.rows = vars;
    Tables[table.name] = table;

    std::string result = langDef->typeGen(table.name, vars);
    return result;
}

void generateFunctionStructs(std::string name, std::string columns)
{
    std::ofstream typeFile("generated_types" + langDef->fileExtension(), std::ios_base::app);
    typeFile << generateStructs(name, columns);
    typeFile.close();
}

void generateTableStructs(pqxx::work &txn)
{
    std::cout << "GENERATING TABLE STRUCTS\n";

    pqxx::result r{txn.exec("SELECT * FROM private.get_tables_and_fields();")};
    std::ofstream typeFile("generated_types" + langDef->fileExtension());
    typeFile << langDef->typeRequirements();

    for (auto row : r)
    {
        std::string name = row["name"].c_str();
        std::string columns = row["fields"].c_str();
        typeFile << generateStructs(name, columns);
    }

    typeFile.close();
    std::cout << "FINISHED TABLE STRUCTS\n";
}

void generateStoredProcedures(pqxx::work &txn)
{
    std::cout << "GENERATING FUNCTIONS\n";

    pqxx::result r{txn.exec("SELECT * FROM private.get_stored_procedures();")};
    std::ofstream procedureFile("generated_procedures" + langDef->fileExtension());

    procedureFile << langDef->procedureRequirements();    

    for (auto row : r)
    {
        std::string output = row["output"].c_str();
        if (output == "")
            procedureFile << generateProcedure(row);
        else
            procedureFile << generateFunction(row);
    }
    std::cout << "FINISHED GENERATING FUNCTIONS\n";
}

std::string generateFunction(pqxx::row &row) // generates code for stored procedures that return a value
{
    std::ostringstream out; 

    std::string functionName = row["name"].c_str();
    std::string returnType = row["output"].c_str();

    if (returnType.substr(0, 5) == "TABLE")
    {
        generateFunctionStructs(functionName, returnType);
        returnType = functionName + "Data";
    }
    else
    {
        returnType = returnType.substr(6) + "Data";
    }

    std::string vars = row["input"].c_str();
    std::vector<VariableData> params = getVariablesAndTypes(vars);

    

    out << langDef->createFunction(functionName, params, Tables[returnType]);

    return out.str(); 
}

std::string generateProcedure(pqxx::row &row) // generates code for stored procedures that do not return a value
{
    std::ostringstream out; 
    std::string functionName = row["name"].c_str();

    std::string vars = row["input"].c_str();
    std::vector<VariableData> params = getVariablesAndTypes(vars);

    out << langDef->createProcedure(functionName, params);

    return out.str(); 
}