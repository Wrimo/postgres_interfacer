#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <pqxx/pqxx>
#include <string>
#include <vector>
#include <map>

struct VariableData
{
    std::string name;
    std::string type;
};

struct TableData
{
    std::string name; 
    std::vector<VariableData> rows; 
};

std::string convertDataType(std::string &);
std::vector<VariableData> getVariablesAndTypes(std::string &);
void dataCleanup(std::string &);

void generateFunctions(pqxx::work &);
void generateStructs(pqxx::work &);

#endif // CODE_GEN_H